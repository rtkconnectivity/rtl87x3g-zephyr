# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
zephyr_include_directories(.)

zephyr_library_sources(soc.c)
zephyr_library_sources_ifdef(CONFIG_REALTEK_BEE_IMAGE_HEADER image_header.c)

zephyr_linker_sources(ROM_START SORT_KEY 0x0header image_header.ld)
zephyr_linker_sources(SECTIONS trace_section.ld)
#workaround for realtek defined sections. Refer to modules\hal\realtek\rtl87x3g\platform\inc\section.h
zephyr_linker_sources(ROM_SECTIONS rtk_flash_code.ld)
zephyr_linker_sources(ITCM_SECTION rtk_ramfunc.ld)
zephyr_linker_sources(SECTIONS dsp_ram_section.ld)#custom sections placed into dsp share ram
zephyr_linker_sources(SECTIONS sram_section.ld)#custom sections placed into sram

if(EXISTS ${APPLICATION_SOURCE_DIR}/VERSION)
zephyr_library_compile_definitions(HAS_APP_VERSION=1)
endif()

set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/arm/cortex_m/scripts/linker.ld CACHE INTERNAL "")

add_subdirectory(osif)

if (CONFIG_PM)
zephyr_library_sources(power.c)
zephyr_code_relocate(FILES power.c LOCATION ITCM)
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/subsys/pm/device.c LOCATION ITCM)
endif()

# Relocate z_isr_install to RAM, because DLPS exit flow would call this before excuting NMI_Handler.
zephyr_code_relocate(LIBRARY arch__common LOCATION ITCM)

# Relocate ISR wrappers and Exceptions to RAM to ensure execution during flash erasing.
# This includes isr_wrapper, z_arm_exc_exit, fault_handler, z_arm_pendsv, etc.
zephyr_code_relocate(LIBRARY arch__arm__core__cortex_m LOCATION ITCM)

# Relocate NMI to RAM for better support DLPS.
# Relocate fault handlers to RAM to ensure execution during flash erasing.
zephyr_code_relocate(LIBRARY arch__arm__core LOCATION ITCM)

# Relocate z_check_stack_sentinel to RAM, because it is invoked by z_arm_exc_exit in ISR context.
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/kernel/thread.c LOCATION ITCM)

# Relocate to RAM to allow systick ISR during flash erasing.
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/drivers/timer/cortex_m_systick.c LOCATION ITCM)

# Keep boot code in XIP to ensure that initial setup routines remain in persistent flash memory.
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/arch/${ARCH}/core/cortex_m/reset.S LOCATION FLASH)
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/arch/${ARCH}/core/cortex_m/prep_c.c LOCATION FLASH)

# Relocated to ITCM for flash suspend mechanism (prevents flash access during PendSV):
# - usage.c: Required when CONFIG_THREAD_ANALYZER=y.
# - spinlock_validate.c: Required when both CONFIG_THREAD_ANALYZER=y && CONFIG_ASSERT=y.
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/kernel/usage.c LOCATION ITCM)
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/kernel/spinlock_validate.c LOCATION ITCM)

if (CONFIG_SOC_NAND_BOOT)
# Reloacte these code on ITCM to ensure DLPS flow when NAND boot is enabled.
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/kernel/kheap.c LOCATION ITCM)
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/lib/heap/heap.c LOCATION ITCM)
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/kernel/sched.c LOCATION ITCM)
zephyr_code_relocate(FILES ${ZEPHYR_BASE}/lib/os/printk.c LOCATION ITCM)
endif ()
