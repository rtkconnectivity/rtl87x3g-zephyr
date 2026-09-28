/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/toolchain.h>

_ASM_FILE_PROLOGUE

GTEXT(__aeabi_read_tp)

GDATA(z_arm_tls_ptr)

/* Grab the TLS pointer and store in R0.
 * According to the Run-Time ABI for the Arm® Architecture section 5.3.5, this
 * function may only clobber r0, ip, lr & CPSR.
 *
 * This can only be guaranteed by either implementing a naked C function with
 * inline assembly, or plain assembly.
 */
#if defined(CONFIG_SOC_SERIES_RTL87X3G) && !defined(CONFIG_SOC_NAND_BOOT)
SECTION_FUNC(text, __aeabi_read_tp)
#else
SECTION_FUNC(TEXT, __aeabi_read_tp)
#endif
	ldr r0, =z_arm_tls_ptr
	ldr r0, [r0]
	bx lr
