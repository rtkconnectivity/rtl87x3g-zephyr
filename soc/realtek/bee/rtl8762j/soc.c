/*
 * Copyright(c) 2024, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/linker/linker-defs.h>
#include <soc.h>

#include <log_core.h>

static int rtl8762j_platform_init(void)
{

	//os_zephyr_patch_init();

	DBG_DIRECT("osif-zephyr init success!");

	extern int sys_clock_driver_init(void);
	sys_clock_driver_init();

	return 0;
}


static int rtl8762j_task_init(void)
{

	return 0;
}

SYS_INIT(rtl8762j_platform_init, EARLY, 0);
SYS_INIT(rtl8762j_task_init, APPLICATION, 0);
