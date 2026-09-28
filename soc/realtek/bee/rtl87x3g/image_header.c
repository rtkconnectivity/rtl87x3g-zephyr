/*
 * Copyright(c) 2024, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <patch_header_check.h>
#include <stdlib.h>
#include <rom_uuid.h>
#include <version.h>
#ifdef HAS_APP_VERSION
#include <app_version.h>
#endif

extern void z_arm_reset(void);

const T_IMG_HEADER_FORMAT img_header __attribute__((section(".image_header"))) = {
	.auth = {
		.img_string = {'I', 'M', 'G', 'H', 'D', 'R'},
		.image_mac = {[0 ... 15] = 0xFF},
	},
	.ctrl_header = {
		.ic_type = 0x13,
		.secure_version = 0,
		.ctrl_flag.load_when_boot = 0,
		.ctrl_flag.not_obsolete = 1,
		.image_id = IMG_MCUAPP,
		.ctrl_flag.integrity_check_en_in_boot = 0,
#ifdef CONFIG_SOC_NAND_BOOT
		.ctrl_flag.device_type = 1,
#endif
	},
#ifdef HAS_APP_VERSION
	.git_ver.sub_version = {
	    ._version_major = APP_VERSION_MAJOR,
        ._version_minor = APP_VERSION_MINOR,
        ._version_revision = APP_PATCHLEVEL,
        ._version_reserve = APP_TWEAK,
    },
#else
	.git_ver.sub_version = {
		._version_major = KERNEL_VERSION_MAJOR,
		._version_minor = KERNEL_VERSION_MINOR,
		._version_revision = KERNEL_PATCHLEVEL,
	},
#endif
	.encrypt_header =
    {
        .ctrl_flag.enc = 0,
    },
	.uuid = DEFINE_symboltable_uuid,
	.exe_entry = (unsigned int)z_arm_reset,
	// .image_base = CONFIG_FLASH_BASE_ADDRESS + CONFIG_FLASH_LOAD_OFFSET,
};
