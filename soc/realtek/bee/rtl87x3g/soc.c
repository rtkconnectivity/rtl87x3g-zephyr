/*
 * Copyright(c) 2024, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/linker/linker-defs.h>
#include <kernel_internal.h>
#include <soc.h>

#include <osif_zephyr.h>
#include "trace.h"
#include <platform_cfg.h>
#include <patch_header_check.h>
#include <patch.h>
#include <rom_symbol.h>
#include <upperstack_compile_stamp.h>
#include <system_status_api.h>
#include <wdg.h>
#include <storage.h>
#include <ftl.h>
#include <os_pm.h>
#include <log_uart_dma.h>
#include <rtl876x_aon_reg.h>
#include <indirect_access.h>
#include <rtl876x_pinmux.h>
#include <test_mode_internal.h>
#include <fmc_api.h>
#include <platform_utils.h>

#include <system_status_api.h>
#ifdef CONFIG_SOC_NAND_BOOT
#include "nftl.h"
#endif

#define VECTOR_TABLE_REGION_RESERVED 0x20000000
#if !defined(CONFIG_SOC_NAND_BOOT)
#define TONE_OFFSET               4096
#else
#define TONE_OFFSET               (4096 + DT_REG_ADDR(DT_NODELABEL(flash_bank0_app_cfg)) + DT_REG_ADDR(DT_ALIAS(vp_data_storage_base)))
#endif
#define TONE_SIZE                 3072

#if DT_NODE_HAS_STATUS(DT_NODELABEL(sram), okay)
extern char __sram_data_start[];
extern char __sram_data_end[];
extern char __sram_data_load_start[];
extern char __sram_bss_start[];
extern char __sram_bss_end[];
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dsp_share_ram), okay)
extern char __dspram_data_start[];
extern char __dspram_data_end[];
extern char __dspram_data_load_start[];
extern char __dspram_bss_start[];
extern char __dspram_bss_end[];
#endif

int32_t ftl_init(const T_STORAGE_PARTITION_INFO *info)
{
    uint32_t ret = 0;

    uint16_t rom_ftl_size = 0x2000;
#ifndef CONFIG_SOC_NAND_BOOT
    ret = ftl_pool_init(info->address, info->size, rom_ftl_size);
    if (ret == 0)
    {
        ftl_cache_init(128);
    }
#else
    ret = ftl_pool_init(0, info->size, 0);
#endif

    return ret;
}

static const T_STORAGE_PARTITION_INFO default_partitions[] =
{
    {
        .name = FTL_PARTITION_NAME,
#ifndef CONFIG_SOC_NAND_BOOT
        .address = DT_REG_ADDR(DT_ALIAS(ftl_region)) + DT_REG_ADDR(DT_ALIAS(ftl_region_storage_base)),
        .size =  DT_REG_SIZE(DT_ALIAS(ftl_region)),
#else
        .address = 0,
        .size =  DT_PROP(DT_ALIAS(nftl_region), size),
#endif
        .perm = STORAGE_PERMISSION_READ,
        .image_id = 0,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_TEXT,
        .init = ftl_init,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
    {
        .name = UPPERSTACK_PARTITION_NAME,
        .address = DT_REG_ADDR(DT_ALIAS(bt_host)) + DT_REG_ADDR(DT_ALIAS(bt_host_storage_base)),
        .size =  DT_REG_SIZE(DT_ALIAS(bt_host)),
        .perm = STORAGE_PERMISSION_READ,
        .image_id = 0,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_TEXT,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
    {
        .name = VP_PARTITION_NAME,
        .address = DT_REG_ADDR(DT_ALIAS(vp_data)) + DT_REG_ADDR(DT_ALIAS(vp_data_storage_base)),
        .size = DT_REG_SIZE(DT_ALIAS(vp_data)),
        .perm = STORAGE_PERMISSION_READ,
        .image_id = FLASH_IMG_VP,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_RO_DATA,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
    {
        .name = TONE_PARTITION_NAME,
        .address = TONE_OFFSET,
        .size = TONE_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .image_id = FLASH_IMG_TONE,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_RO_DATA,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
};

static void rtl87x3g_vector_table_relocation(void)
{
/*
 * Before Vector Table Relocation:
 * The RTL87X3G initialization process in rom updates vector entries in the RamVectorTable.
 * Therefore, we need to register these ISRs into Zephyr's interrupt system.
 *
 * Note:
 * Make sure skip the first 16 system exception vectors.
 */
	uint32_t *RamVectorTable_INT = (uint32_t *)(0x20000000 + 16 * 4);

	for (int irq = 0; irq < CONFIG_NUM_IRQS; irq++) {
		if (RamVectorTable_INT[irq] != (uint32_t)NULL) {
			if (NVIC_GetEnableIRQ(irq) == 1) {
				NVIC_DisableIRQ(irq);
				z_isr_install(irq, (void *)RamVectorTable_INT[irq], NULL);
				NVIC_EnableIRQ(irq);
			} else {
				z_isr_install(irq, (void *)RamVectorTable_INT[irq], NULL);
			}
		}
	}

/* Vector Table Relocation:
 * Copy zephyr vector table to 0x20000000(reserved in RAM region by ROM) because flash suspend/resume need
 * all the ISR entry flow on the RAM including the Vector Table Loaction.
 * Do RamVectorTableUpdate patch in os_zephyr_patch_init.
 */
	size_t vector_size = (size_t)_vector_end - (size_t)_vector_start;
	(void)memcpy((void *)VECTOR_TABLE_REGION_RESERVED, _vector_start, vector_size);
	SCB->VTOR = (uint32_t)VECTOR_TABLE_REGION_RESERVED;
}

#if CONFIG_LOG_RTK && !CONFIG_MINIMAL_LIBC
int printf(const char *fmt, ...)
{
    extern void log_direct(const char *fmt, ...);
	log_direct(fmt);
    return 0;
}
#endif

static void rtl87x3g_extra_ram_init(void)
{
#if DT_NODE_HAS_STATUS(DT_NODELABEL(sram), okay)
    z_early_memcpy(&__sram_data_start, &__sram_data_load_start,
		       __sram_data_end - __sram_data_start);

    z_early_memset(__sram_bss_start, 0, __sram_bss_end - __sram_bss_start);
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dsp_share_ram), okay)
    z_early_memcpy(&__dspram_data_start, &__dspram_data_load_start,
		       __dspram_data_end - __dspram_data_start);

    z_early_memset(__dspram_bss_start, 0, __dspram_bss_end - __dspram_bss_start);
#endif

}

static int rtl87x3g_platform_init(void)
{
    rtl87x3g_extra_ram_init();

    rtl87x3g_vector_table_relocation();

	os_zephyr_patch_init();

	DBG_DIRECT("osif-zephyr init success!");

    uint32_t platform_addr = *get_image_exe_addr_in_bank(ota_header_addr_rom, IMG_SYSPATCH);

    IMG_CHECK_ERR_TYPE img_check_result = image_entry(IMG_SYSPATCH, platform_addr);

    if (img_check_result != IMG_CHECK_PASS)
    {
        DIRECT_LOG("IMG ERR, id: 0x%x, err: 0x%x", 2, IMG_SYSPATCH, img_check_result);
    }

    set_up_wdt();

    enable_rxi300_interrupt();

    si_flow_data_init();

    ft_paras_apply();

	AON_FAST_BOOT_TYPE aon_fast_boot = {.d16 = btaon_fast_read(AON_FAST_BOOT)};
    bool aon_boot_done = aon_fast_boot.aon_boot_done;

    dvfs_register_check_func(check_pke_ram_idle);//os_mem_alloc

    /************** set_up_log_buf() start **************/
    log_module_trace_init(NULL);
    log_buf_init();
    log_uart_dma_init();
    sys_timestamp_init();//os_mem_alloc
    /************** set_up_log_buf() end **************/

    set_active_mode_clk_src();

    set_up_32k_clk_src();//os_mem_alloc

    pmu_apply_voltage_tune();

    if (!aon_boot_done)
    {
        pmu_power_on_sequence_restart();

        set_reg_by_otp(T_POWER_ON_SEQ);

        DIRECT_LOG("SYS ROM Version: 0x%x", 1, VERSION_GCID);

        Pad_ClearAllWakeupINT();
    }
    else
    {
        si_flow_after_exit_low_power_mode();

        pmu_active_ctrl();
    }

    aon_fast_boot.aon_boot_done = 1;
    btaon_fast_write(AON_FAST_BOOT, aon_fast_boot.d16);

    disable_unused_clock();

    ram_ctrl_power_set();

    hal_setup_hardware();

    hal_setup_cpu();

/* Enable cortex-m systick clock src */
    extern void (*systick_clk_src_setup)(void);
    systick_clk_src_setup();

	return 0;
}

static int rtl87x3g_update_systick_config(void)
{
	/* rtl87x3g's cortex-m systick timer is using external clock source instead of cpu clock as
	 * referance. The priority of systick interrupt is lowest for rtl87x3g SoCs.
	 */
	NVIC_SetPriority(SysTick_IRQn, 0xff);
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;

	return 0;
}


static int rtl87x3g_task_init(void)
{
/* main() in main_full.c */
    main_patch();
// sd_register_mem_func(os_sd_alloc_mem, os_mem_free);
// sd_mem_copy(SDHC_ID1);
    mcu_test_mode_setting(&mcu_test_mode_cfg);

    platform_rtc_aon_init();

    power_manager_master_init();

    power_manager_slave_init();

    platform_pm_init();

	os_pm_init();

	init_osc_sdm_timer();

	phy_hw_control_init(false);
	phy_init(false);

	thermal_tracking_timer_init();

    uint32_t random_seed = platform_random(0xFFFFFFFF);
    srand(random_seed);

    if (sys_init_cfg.lowerstack_en)
    {
        extern T_ROM_HEADER_FORMAT boot_rom_header;
        T_ROM_HEADER_FORMAT *stack_header = (T_ROM_HEADER_FORMAT *)(DT_REG_ADDR(DT_NODELABEL(stack_rom)));

        if (memcmp(stack_header->uuid, boot_rom_header.uuid, UUID_SIZE) == 0)
        {
#ifdef CONFIG_REALTEK_BEE_FPGA
            test_ahb_wait_cnt_config();
#endif
            VOID_PATCH_FUNC low_stack_init = (VOID_PATCH_FUNC)((uint32_t)stack_header->entry_ptr);

            low_stack_init();

			DBG_DIRECT("LOWERSTACK init success!");
        } else {
			DBG_DIRECT("LOWERSTACK init fail!");
		}
    } else {
		DBG_DIRECT("sys_init_cfg.lowerstack_en not enabled!");
	}

    if (flash_nor_get_exist(0))  /* Nor flash controlled by SPIC0 */
    {
        flash_nor_dump_main_info();
        flash_nor_cmd_list_init();
        flash_nor_malloc_for_query_info(0);/* Nor flash controlled by SPIC0 */
        flash_nor_init_bp_lv();
        flash_task_init();
    }

    hw_aes_mutex_init();

    dvfs_init();

    adapter_init();

    adc_mgr_init(sys_init_cfg.adc_mgr_queue);

    charger_system_init();

    system_interrupt_init();

    AON_FAST_BOOT_TYPE aon_fast_boot = {.d16 = btaon_fast_read(AON_FAST_BOOT)};
    aon_fast_boot.pon_boot_done = 1;
    btaon_fast_write(AON_FAST_BOOT, aon_fast_boot.d16);

    patch_fw_sim();

    memory_watch_enable();

#if DT_NODE_EXISTS(DT_NODELABEL(nand_flash))
    extern void (*flash_nand_set_bp_lv)(uint32_t idx, uint8_t bp_lv);
    flash_nand_set_bp_lv(0, 0);
    bool ret = nftl_init(DT_REG_ADDR(DT_ALIAS(ftl_region)) + DT_REG_ADDR(DT_ALIAS(ftl_region_storage_base)), DT_REG_SIZE(DT_ALIAS(ftl_region)));
	DBG_DIRECT("nftl init %s", ret ? "success" : "fail");
#endif
    storage_partition_init(default_partitions,
                    sizeof(default_partitions) / sizeof(default_partitions[0]));
	if (sys_init_cfg.stack_en)
    {
		uint8_t upperstack_compile_stamp[16] = DEFINE_upperstack_compile_stamp;
    	sys_hall_upperstack_ini(upperstack_compile_stamp);
        DBG_DIRECT("UPPERSTACK init success!");
    } else {
		DBG_DIRECT("sys_init_cfg.stack_en not enabled!");
	}

    if (is_mcu_test_mode(&mcu_test_mode_cfg))
    {
        DBG_DIRECT("SKIP APP!!!");
        k_thread_suspend(_current);
    }

	return 0;
}

#ifdef CONFIG_PLATFORM_SPECIFIC_INIT
void z_arm_platform_init(void)
{
#ifdef CONFIG_DSP_SHARE_RAM_80K
    sys_hall_set_dsp_share_memory(SHARE_DSP_RAM_80K);
#endif
#ifdef CONFIG_DSP_SHARE_RAM_160K
    sys_hall_set_dsp_share_memory(SHARE_DSP_RAM_160K);
#endif
#ifdef CONFIG_DSP_SHARE_RAM_208K
    sys_hall_set_dsp_share_memory(SHARE_DSP_RAM_208K);
#endif
#ifdef CONFIG_DSP_SHARE_RAM_448K
    sys_hall_set_dsp_share_memory(SHARE_DSP_RAM_448K);
#endif
}
#endif

void sys_arch_reboot(int type)
{
	/* Convert SYS_REBOOT_WARM (0) to RESET_ALL_EXCEPT_AON (1).
	 * Convert SYS_REBOOT_COLD (1) to RESET_ALL (3).
	 */
	T_WDG_MODE wdg_mode = (type == SYS_REBOOT_WARM) ? RESET_ALL_EXCEPT_AON : RESET_ALL;

	chip_reset(wdg_mode);
}

SYS_INIT(rtl87x3g_platform_init, EARLY, 0);
SYS_INIT(rtl87x3g_update_systick_config, PRE_KERNEL_2, 1);
SYS_INIT(rtl87x3g_task_init, APPLICATION, 0);
