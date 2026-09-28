# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
zephyr_library_sources_ifdef(CONFIG_REALTEK_OSIF osif_zephyr.c)
zephyr_code_relocate(FILES osif_zephyr.c LOCATION ITCM) #for z_arm_nmi
zephyr_include_directories(.)