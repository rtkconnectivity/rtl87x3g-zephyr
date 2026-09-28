/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_cctl

#include <stdint.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/clock_control.h>

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include <rtl876x_rcc.h>
#endif

#include <trace.h>
#define DBG_DIRECT_SHOW 0

struct clock_control_rtl87x3g_config {
	uint32_t reg;
};

typedef struct {
	uint32_t apbperiph;
	uint32_t apbperiph_clk;
} apb_cfg;

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
static const apb_cfg bee_apb_table[] = {
	{APBPeriph_ZIGBEE, APBPeriph_ZIGBEE_CLOCK},
	{APBPeriph_GMAC, APBPeriph_GMAC_CLOCK},
	{APBPeriph_JPEG, APBPeriph_JPEG_CLOCK},
	{APBPeriph_CAN1, APBPeriph_CAN1_CLOCK},
	{APBPeriph_IDU, APBPeriph_IDU_CLOCK},
	{APBPeriph_PKE, APBPeriph_PKE_CLOCK},
	{APBPeriph_PPE, APBPeriph_PPE_CLOCK},
	{APBPeriph_CAN0, APBPeriph_CAN0_CLOCK},
	{APBPeriph_TIMER1, APBPeriph_TIMER1_CLOCK},
	{APBPeriph_PDCK, APBPeriph_CKE_PDCK_CLOCK},
	{APBPeriph_SWR_SS, APBPeriph_SWR_SS_CLOCK},
	{APBPeriph_CAN2, APBPeriph_CAN2_CLOCK},
	{APBPeriph_TIMER, APBPeriph_TIMER_CLOCK},
	{APBPeriph_TIMERA, APBPeriph_TIMERA_CLOCK},
	{APBPeriph_SD_HOST, APBPeriph_SD_HOST_CLOCK},
	{APBPeriph_GDMA, APBPeriph_GDMA_CLOCK},
	{APBPeriph_UART5, APBPeriph_UART5_CLOCK},
	{APBPeriph_UART4, APBPeriph_UART4_CLOCK},
	{APBPeriph_UART3, APBPeriph_UART3_CLOCK},
	{APBPeriph_UART2, APBPeriph_UART2_CLOCK},
	{APBPeriph_UART1, APBPeriph_UART1_CLOCK},
	{APBPeriph_UART0, APBPeriph_UART0_CLOCK},
	{APBPeriph_FLASH2, APBPeriph_FLASH2_CLOCK},
	{APBPeriph_FLASH1, APBPeriph_FLASH1_CLOCK},
	{APBPeriph_FLASH, APBPeriph_FLASH_CLOCK},
	{APBPeriph_FLASH3, APBPeriph_FLASH3_CLOCK},
	{APBPeriph_BTBUS, APBPeriph_BTBUS_CLOCK},
	{APBPeriph_SD_HOST1, APBPeriph_SD_HOST1_CLOCK},
	{APBPeriph_2P4G, APBPeriph_2P4G_CLOCK},
	{APBPeriph_EFUSE, APBPeriph_EFUSE_CLOCK},
	{APBPeriph_DSP_WDT, APBPeriph_CKE_DSP_WDT_CLOCK},
	{APBPeriph_ASRC, APBPeriph_ASRC_CLOCK},
	{APBPeriph_DSP_MEM, APBPeriph_DSP_MEM_CLOCK},
	{APBPeriph_SPI0_SLAVE, APBPeriph_SPI0_SLAVE_CLOCK},
	{APBPeriph_I2C2, APBPeriph_I2C2_CLOCK},
	{APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK},
	{APBPeriph_QDEC, APBPeriph_QDEC_CLOCK},
	{APBPeriph_I2C1, APBPeriph_I2C1_CLOCK},
	{APBPeriph_I2C0, APBPeriph_I2C0_CLOCK},
	{APBPeriph_SPI2, APBPeriph_SPI2_CLOCK},
	{APBPeriph_IR, APBPeriph_IR_CLOCK},
	{APBPeriph_SPI1, APBPeriph_SPI1_CLOCK},
	{APBPeriph_SPI0, APBPeriph_SPI0_CLOCK},
	{APBPeriph_DISP, APBPeriph_DISP_CLOCK},
	{APBPeriph_SIMC, APBPeriph_SIMC_CLOCK},
	{APBPeriph_ISO7816, APBPeriph_ISO7816_CLOCK},
	{APBPeriph_RNG, APBPeriph_RNG_CLOCK},
	{APBPeriph_AES, APBPeriph_AES_CLOCK},
	{APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK},
	{APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK},
	{APBPeriph_ADC, APBPeriph_ADC_CLOCK},
	{APBPeriph_I2S2, APBPeriph_I2S2_CLOCK},
	{APBPeriph_I2S1, APBPeriph_I2S1_CLOCK},
	{APBPeriph_I2S0, APBPeriph_I2S0_CLOCK},
	{APBPeriph_CODEC, APBPeriph_CODEC_CLOCK},
	{APBPeriph_CKE_MODEM, APBPeriph_CKE_MODEM_CLOCK},
	{APBPeriph_VENDOR_REG, APBPeriph_VENDOR_REG_CLOCK},
	{APBPeriph_CKE_BTV, APBPeriph_CKE_BTV_CLOCK},
	{APBPeriph_BUS_RAM_SLP, APBPeriph_BUS_RAM_SLP_CLOCK},
	{APBPeriph_CKE_CTRLAP, APBPeriph_CKE_CTRLAP_CLOCK},
	{APBPeriph_CKE_PLFM, APBPeriph_CKE_PLFM_CLOCK},
	{APBPeriph_AHBC, APBPeriph_AHBC_CLOCK},
#endif
};

static int clock_control_rtl87x3g_on(const struct device *dev, clock_control_subsys_t sys)
{
	uint16_t id = *(uint16_t *)sys;

	RCC_PeriphClockCmd(bee_apb_table[id].apbperiph, bee_apb_table[id].apbperiph_clk, ENABLE);
#if DBG_DIRECT_SHOW
	DBG_DIRECT("[%s] sys=%d, apbperiph=0x%x, apbperiph_clk=0x%x", __func__, id,
		   bee_apb_table[id].apbperiph, bee_apb_table[id].apbperiph_clk);
#endif
	return 0;
}

static int clock_control_rtl87x3g_off(const struct device *dev, clock_control_subsys_t sys)
{
	uint16_t id = *(uint16_t *)sys;

	RCC_PeriphClockCmd(bee_apb_table[id].apbperiph, bee_apb_table[id].apbperiph_clk, DISABLE);

#if DBG_DIRECT_SHOW
	DBG_DIRECT("[%s] sys=%d, apbperiph=%d, apbperiph_clk=%d", __func__, sys,
		   bee_apb_table[id].apbperiph, bee_apb_table[id].apbperiph_clk);
#endif
	return 0;
}

static struct clock_control_driver_api clock_control_rtl87x3g_api = {
	.on = clock_control_rtl87x3g_on,
	.off = clock_control_rtl87x3g_off,
};

static const struct clock_control_rtl87x3g_config config = {
	.reg = DT_REG_ADDR(DT_INST_PARENT(0)),
};

DEVICE_DT_INST_DEFINE(0, NULL, NULL, NULL, &config, PRE_KERNEL_1,
		      CONFIG_CLOCK_CONTROL_INIT_PRIORITY, &clock_control_rtl87x3g_api);
