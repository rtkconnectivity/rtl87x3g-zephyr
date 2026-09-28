# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=RTL87X3G" "--speed=4000")
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
# mpcli flash runner for rtl87x3g_watch.
#
# The defaults below only cover what is board-fixed and host/OS independent:
#   --ic-type=RTL87X3G : mpcli -T, mandatory for the 87x3 mpcli.
#   --reset            : mpcli -r, reboot the board after flashing.
#
# The serial port and the RTS auto-reset are host/OS specific and are NOT
# wired here -- pass them on the `west flash` command line:
#
#   --port <serial>      Flash port. REQUIRED (no default here).
#                          Linux:   --port=/dev/ttyUSB0
#                          Windows: --port=COM3
#   --auto               Also launch the RTS reset helper so a board wired with
#                        an RTS->reset auto-download circuit enters download
#                        mode with no manual reset. Requires --auto-script.
#   --auto-reset-port    Port whose RTS drives reset, if it is on a different
#                        adapter than --port (defaults to --port).
#
#   Linux command example:
#     west flash --port=/dev/ttyUSB0 --auto \
#   Windows command example:
#   --auto-reset-port:
#     west flash --port=COM3 --auto ^
#       --auto-reset-port=COM4
#
# jlink is the default `west flash` runner: it is selected by the
# board_set_flasher_ifnset(jlink) call inside jlink.board.cmake included below
# (board_set_flasher_ifnset is first-wins, and mpcli deliberately does NOT
# claim the default here). mpcli stays reachable via `west flash --runner mpcli`.
board_runner_args(mpcli "--ic-type=RTL87X3G" "--reset" "--auto-script=../../../realtek-app/tools/mpcli/flash_autoreset.py")
board_finalize_runner_args(mpcli)
