/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_RTL87X3G_CLOCKS_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_RTL87X3G_CLOCKS_H_

/**
 * @name Register offsets
 * @{
 */

/** @} */

/**
 * @name Clock enable/disable definitions for peripherals
 * @{
 */
#define APB_CLK(peri) APBPeriph_##peri##_CLOCK

#define APBPeriph_ZIGBEE_CLOCK          (0)
#define APBPeriph_GMAC_CLOCK            (1)
#define APBPeriph_JPEG_CLOCK            (2)
#define APBPeriph_CAN1_CLOCK            (3)
#define APBPeriph_RTZIP_CLOCK           (4)
#define APBPeriph_PKE_CLOCK             (5)
#define APBPeriph_PPE_CLOCK             (6)
#define APBPeriph_CAN0_CLOCK            (7)
#define APBPeriph_TIMER1_CLOCK          (8)
#define APBPeriph_PDCK_CLOCK            (9)
#define APBPeriph_SWR_SS_CLOCK          (10)
#define APBPeriph_CAN2_CLOCK            (11)
#define APBPeriph_TIMER_CLOCK           (12)
#define APBPeriph_TIMERA_CLOCK          (13)
#define APBPeriph_SD_HOST_CLOCK         (14)
#define APBPeriph_GDMA_CLOCK            (15)
#define APBPeriph_UART5_CLOCK           (16)
#define APBPeriph_UART4_CLOCK           (17)
#define APBPeriph_UART3_CLOCK           (18)
#define APBPeriph_UART2_CLOCK           (19)
#define APBPeriph_UART1_CLOCK           (20)
#define APBPeriph_UART0_CLOCK           (21)
#define APBPeriph_FLASH2_CLOCK          (22)
#define APBPeriph_FLASH1_CLOCK          (23)
#define APBPeriph_FLASH_CLOCK           (24)
#define APBPeriph_FLASH3_CLOCK          (25)
#define APBPeriph_BTBUS_CLOCK           (26)
#define APBPeriph_SD_HOST1_CLOCK        (27)
#define APBPeriph_2P4G_CLOCK            (28)
#define APBPeriph_EFUSE_CLOCK           (29)
#define APBPeriph_DSP_WDT_CLOCK         (30)
#define APBPeriph_ASRC_CLOCK            (31)
#define APBPeriph_DSP_MEM_CLOCK         (32)
#define APBPeriph_SPI0_SLAVE_CLOCK      (33)
#define APBPeriph_I2C2_CLOCK            (34)
#define APBPeriph_KEYSCAN_CLOCK         (35)
#define APBPeriph_QDEC_CLOCK            (36)
#define APBPeriph_I2C1_CLOCK            (37)
#define APBPeriph_I2C0_CLOCK            (38)
#define APBPeriph_SPI2_CLOCK            (39)
#define APBPeriph_IR_CLOCK              (40)
#define APBPeriph_SPI1_CLOCK            (41)
#define APBPeriph_SPI0_CLOCK            (42)
#define APBPeriph_DISP_CLOCK            (43)
#define APBPeriph_SIMC_CLOCK            (44)
#define APBPeriph_ISO7816_CLOCK         (45)
#define APBPeriph_RNG_CLOCK             (46)
#define APBPeriph_AES_CLOCK             (47)
#define APBPeriph_GPIOB_CLOCK           (48)
#define APBPeriph_GPIOA_CLOCK           (49)
#define APBPeriph_ADC_CLOCK             (50)
#define APBPeriph_I2S2_CLOCK            (51)
#define APBPeriph_I2S1_CLOCK            (52)
#define APBPeriph_I2S0_CLOCK            (53)
#define APBPeriph_CODEC_CLOCK           (54)
#define APBPeriph_CKE_MODEM_CLOCK       (55)
#define APBPeriph_VENDOR_REG_CLOCK      (56)
#define APBPeriph_CKE_BTV_CLOCK         (57)
#define APBPeriph_BUS_RAM_SLP_CLOCK     (58)
#define APBPeriph_CKE_CTRLAP_CLOCK      (59)
#define APBPeriph_CKE_PLFM_CLOCK        (60)
#define APBPeriph_AHBC_CLOCK            (61)

/** @} */

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_CLOCK_RTL87X3G_CLOCKS_H_ */
