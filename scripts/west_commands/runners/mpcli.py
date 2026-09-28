# Copyright (c) 2017 Linaro Limited.
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: Apache-2.0

'''Runner for flashing bee devices with mpcli.'''

import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
from textwrap import dedent

from runners.core import FileType, RunnerCaps, ZephyrBinaryRunner


class MPCLIBinaryRunner(ZephyrBinaryRunner):
    '''Runner front-end for mpcli.'''

    def __init__(self, cfg, port, build_dir, bin_address, chip_erase, mp_json, reset,
                 ic_type=None, auto=False, auto_script=None,
                 auto_reset_port=None):
        super().__init__(cfg)
        self.port = port
        self.build_dir = build_dir
        self.bin_address = bin_address
        self.chip_erase = chip_erase
        self.mp_json = mp_json
        self.baud = "1000000"
        self.reset = reset
        self.ic_type = ic_type
        self.auto = auto
        self.auto_script = auto_script
        self.auto_reset_port = auto_reset_port
        self.app_bin_file = cfg.bin_file
        self.ext_file = cfg.file
        self.ext_file_type = cfg.file_type
        self.files: list[dict[str, any]] = []

    @classmethod
    def name(cls):
        return 'mpcli'

    @classmethod
    def capabilities(cls):
        return RunnerCaps(commands={'flash'}, file=True, erase=True, reset=True)

    @classmethod
    def do_add_parser(cls, parser):
        mpcli_parser = parser

        mpcli_parser.add_argument(
            '--port',
            required=False,
            type=str,
            help='Serial communication port (e.g., COM3, /dev/ttyUSB0)',
        )
        mpcli_parser.add_argument(
            '--ic-type',
            dest='ic_type',
            type=str,
            help='mpcli ic type passed as -T (e.g., RTL87X3G). Required by the '
                '87x3-series mpcli; omit for tools that auto-detect.',
        )
        # --- Local extension: RTS auto-reset. This runner still only runs mpcli;
        # with --auto it additionally launches --auto-script, which owns the RTS
        # reset entirely (see flash_autoreset.py). ---------------------------
        mpcli_parser.add_argument(
            '--auto',
            action='store_true',
            help='Also run the RTS reset helper (--auto-script) so a board wired '
                'with an RTS->reset auto-download circuit (e.g. rtl87x3g_watch) '
                'enters download mode without a manual reset.',
        )
        mpcli_parser.add_argument(
            '--auto-script',
            dest='auto_script',
            type=str,
            help='Path to the RTS reset helper (flash_autoreset.py). Required '
                'when --auto is set (usually wired by board.cmake).',
        )
        mpcli_parser.add_argument(
            '--auto-reset-port',
            dest='auto_reset_port',
            type=str,
            help='Serial port whose RTS drives reset, if different from the flash '
                '--port. Passed on to the auto-script; defaults to --port.',
        )
        group = mpcli_parser.add_mutually_exclusive_group()
        group.add_argument(
            '--bin-address',
            type=str,
            help='Download address(hex format, e.g., 0x8000000)',
        )
        group.add_argument(
            '--mp-json',
            type=str,
            help=dedent('''
                        Configuration json file containing binary path and download address.
                        When not provided, it will be generated on-the-fly from the other arguments.
                        Note: The port setting can be omitted from the JSON configuration if
                        it is provided via the --port option.
                        Example format:
                        {
                            "mptoolconfig": {
                                "port": "/dev/ttyX",
                                "baud": "1000000",
                                "appimage": {
                                    "relativepath": "directory path for `file` objects, relative to
                                                     this configuration file",
                                    "file": [
                                        {
                                            "id": 0,
                                            "address": "0x00801000",
                                            "name": "fw1.bin",
                                            "enable": "1"
                                        },
                                        {
                                            "id": 1,
                                            "address": "0x00802000",
                                            "name": "fw2.bin",
                                            "enable": "1"
                                        },
                                        ...
                                    ]
                                }
                            }
                        }
                        '''),
        )
        return parser

    @classmethod
    def do_create(cls, cfg, args):
        return MPCLIBinaryRunner(
            cfg,
            args.port,
            build_dir=cfg.build_dir,
            bin_address=args.bin_address,
            chip_erase=args.erase,
            mp_json=args.mp_json,
            reset=args.reset,
            ic_type=args.ic_type,
            auto=args.auto,
            auto_script=args.auto_script,
            auto_reset_port=args.auto_reset_port,
        )

    def create_mptool_config(self, config_filename: str) -> None:
        """
        Create MP tool configuration to a JSON file.
        """
        if not self.port:
            raise ValueError('Cannot flash; --port is required')
        mptool_config = {
            "mptoolconfig": {
                "port": self.port,  # serial port
                "baud": self.baud,  # baud rate
                "appimage": {"relativepath": "", "file": self.files},
            }
        }
        with open(config_filename, 'w', encoding='utf-8') as f:
            json.dump(mptool_config, f, indent=4, ensure_ascii=False)

    def add_file(self, address: str, name: str, id: int = 0, enable: str = "1") -> None:
        file_item = {"id": id, "address": address, "name": name, "enable": enable}
        self.files.append(file_item)

    def execute_mpcli(self, mptoolconfig_path):
        """
        Execute mpcli command with the given configuration file.
        """
        cmd_args = [
            'mpcli',
            '-f',
            mptoolconfig_path,
            '-a',
            '-M',
            '5',
        ]

        if self.ic_type:
            cmd_args.extend(['-T', self.ic_type])

        if self.port:
            cmd_args.extend(['-c', self.port])

        if self.reset:
            cmd_args.extend(['-r'])

        if self.chip_erase:
            cmd_args.extend(["-E"])

        # mpcli resolves its bundled ram-patch firmware (fw/<ic>/*.bin) relative
        # to the current working directory, not to its own binary. Run it from
        # the directory that holds the mpcli executable so fw/ is found.
        exe = shutil.which('mpcli')
        run_cwd = os.path.dirname(os.path.realpath(exe)) if exe else None
        self.check_call(cmd_args, cwd=run_cwd)

    def get_flash_parameters(self):
        """Get file path and download address for flashing."""
        if self.ext_file:
            # Use external binary file
            if self.ext_file_type != FileType.BIN:
                raise ValueError('Cannot flash; mpcli runner only supports bin type')

            bin_file_path = Path(self.ext_file)
        else:
            # Use build system generated file
            bin_file_path = Path(self.app_bin_file)

        download_address = self.bin_address or hex(
            self.flash_address_from_build_conf(self.build_conf)
        )

        return bin_file_path, download_address

    def run_autoreset(self):
        """Launch the RTS reset helper (flash_autoreset.py) in the background.

        The helper owns the RTS reset entirely: it waits briefly (so mpcli is
        already sending handshakes), pulses RTS to drop the board into the MP
        loader, then exits. This runner does not touch RTS itself. It runs
        concurrently with mpcli because the board does not wait in the loader --
        the reset must land while mpcli is handshaking.
        """
        if not self.auto_script:
            raise ValueError(
                'Cannot flash; --auto requires --auto-script <flash_autoreset.py> '
                '(normally wired by board.cmake)')
        script = Path(self.auto_script)
        if not script.is_file():
            raise ValueError(f'--auto-script not found: {script}')
        auto_reset_port = self.auto_reset_port or self.port
        if not auto_reset_port:
            raise ValueError('Cannot flash; --auto needs --auto-reset-port or --port')
        self.logger.info('RTS auto-reset via %s on %s', script, auto_reset_port)
        return subprocess.Popen([sys.executable, str(script), '--port', auto_reset_port])

    def do_run(self, command, **kwargs):
        self.require('mpcli')

        if self.mp_json:
            # Execute mpcli using the provided configuration file
            mptoolconfig_path = self.mp_json
        else:
            # Create the configuration file and execute mpcli using it
            bin_file_path, download_address = self.get_flash_parameters()

            self.add_file(download_address, bin_file_path.name)
            mptoolconfig_path = str(bin_file_path.parent / "mptoolconfig.json")
            self.create_mptool_config(mptoolconfig_path)

        # --auto: kick off the RTS reset helper first (it runs concurrently and
        # resets the board into download mode while mpcli handshakes), then run
        # mpcli. The finally makes sure the background helper is cleaned up even
        # if execute_mpcli raises.
        pulser = self.run_autoreset() if self.auto else None
        try:
            self.execute_mpcli(mptoolconfig_path)
        finally:
            if pulser is not None and pulser.poll() is None:
                pulser.terminate()
