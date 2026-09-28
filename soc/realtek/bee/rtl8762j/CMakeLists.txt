# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
zephyr_include_directories(.)

zephyr_library_sources(soc.c)
zephyr_library_sources_ifdef(CONFIG_REALTEK_BEE_IMAGE_HEADER image_header.c)

zephyr_linker_sources(ROM_START SORT_KEY 0x0header image_header.ld)
# zephyr_linker_sources(SECTIONS trace_section.ld)

set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/arm/cortex_m/scripts/linker.ld CACHE INTERNAL "")

#add_subdirectory(osif)
add_subdirectory(platform)
