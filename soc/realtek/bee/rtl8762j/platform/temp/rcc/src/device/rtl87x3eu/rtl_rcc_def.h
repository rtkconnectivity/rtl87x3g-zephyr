/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl_rcc_def.h
* @brief     header file of reset and clock control driver.
* @details
* @author   colin
* @date     2024-06-14
* @version  v1.0
* *********************************************************************************************************
*/


#ifndef RTL_RCC_DEF_H
#define RTL_RCC_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"

/*============================================================================*
 *                         RCC Features
 *============================================================================*/

#define RCC_SUPPORT_CLOCKGATECMD_API                   (0)
#define RCC_SUPPORT_AUTO_CLOCK                         (0)

/*============================================================================*
 *                         RCC Constants
 *============================================================================*/

/** @defgroup RCC_Peripheral_Clock  RCC Peripheral Clock
  * @{
  */
/*start  offset address  28| --> bit28, (0x01 << 29) --> adress 0x230,(0x02 << 29) -->0x234, 0x00 << 10) -->0x00 sleep clock cfg?  yes*/
#define APBPeriph_SLEEP_CLOCK_EXIST       (0)
#define APBPeriph_SLEEP_CLOCK_NOT_EXIST   (BIT10)

/* 0x220 */
#define APBPeriph_I2S2_CLOCK              ((uint32_t)(1 << 12))
#define APBPeriph_I2S1_CLOCK              ((uint32_t)((1 << 6) | (1 << 8)))
#define APBPeriph_I2S0_CLOCK              ((uint32_t)((1 << 5) | (1 << 8)))
#define APBPeriph_CODEC_CLOCK             ((uint32_t)(1 << 4))

/* 0x230 */
#define APBPeriph_SD_HOST1_CLOCK            ((uint32_t)( 28 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SD_HOST_CLOCK             ((uint32_t)( 26 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GPIOA_CLOCK               ((uint32_t)( 24 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GPIO_CLOCK                APBPeriph_GPIOA_CLOCK
#define APBPeriph_GPIOB_CLOCK               ((uint32_t)( 22 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_FLASH2_CLOCK              ((uint32_t)( 20 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_FLASH1_CLOCK              ((uint32_t)( 18 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DMA_CLOCK                ((uint32_t)( 16 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_TIMER_CLOCK               ((uint32_t)( 14 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_TIMERA_CLOCK              APBPeriph_TIMER_CLOCK
#define APBPeriph_FLASH3_CLOCK              ((uint32_t)( 12 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_AHBC_CLOCK                ((uint32_t)( 10 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_FLASH_CLOCK               ((uint32_t)(  8 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_VENDOR_REG_CLOCK          ((uint32_t)(  6 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CKE_BTV_CLOCK             ((uint32_t)(  5 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_BUS_RAM_SLP_CLOCK         ((uint32_t)(  4 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_CTRLAP_CLOCK          ((uint32_t)(  3 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_PLFM_CLOCK            ((uint32_t)(  2 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_GPIO1_DEB_CLOCK           ((uint32_t)(  1 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_GPIO0_DEB_CLOCK           ((uint32_t)(  0 | (0x01UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))

/* 0x234 */
#define APBPeriph_RTZIP_CLOCK               ((uint32_t)( 30 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_PKE_CLOCK                 ((uint32_t)( 28 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_PPE_CLOCK                 ((uint32_t)( 26 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CAN0_CLOCK                ((uint32_t)( 24 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SPI2_CLOCK                ((uint32_t)( 22 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_IR_CLOCK                  ((uint32_t)( 20 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SPI1_CLOCK                ((uint32_t)( 18 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SPI0_CLOCK                ((uint32_t)( 16 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_TIMER1_CLOCK              ((uint32_t)( 14 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_GMAC_CLOCK                ((uint32_t)( 13 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_SM3_CLOCK             ((uint32_t)( 12 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_SHA256_CLOCK          ((uint32_t)( 11 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_AAC_XTAL_CLOCK        ((uint32_t)( 10 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_PDCK_CLOCK            ((uint32_t)(  9 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_RNG_CLOCK                 ((uint32_t)(  8 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_SWR_SS_CLOCK              ((uint32_t)(  6 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CAL_32K_CLOCK             ((uint32_t)(  5 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_MODEM_CLOCK           ((uint32_t)(  4 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_SPI0_SLAVE_CLOCK          ((uint32_t)(  2 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_2P4G_CLOCK                ((uint32_t)(  0 | (0x02UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

/* 0x238 */
#define APBPeriph_EFUSE_CLOCK             ((uint32_t)( 31 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_DSP_WDT_CLOCK       ((uint32_t)( 30 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_NOT_EXIST))
#define APBPeriph_CKE_DSP_CLOCK           ((uint32_t)( 28 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CKE_H2D_D2H             ((uint32_t)( 26 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ADC_CLOCK               ((uint32_t)( 24 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DSP_MEM_CLOCK           ((uint32_t)( 22 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ASRC_CLOCK              ((uint32_t)( 20 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DISP_CLOCK              ((uint32_t)( 18 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DATA_MEM1_CLOCK         ((uint32_t)( 16 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_DATA_MEM0_CLOCK         ((uint32_t)( 14 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_I2C2_CLOCK              ((uint32_t)( 12 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_SIMC_CLOCK              ((uint32_t)( 10 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ISO7816_CLOCK           APBPeriph_SIMC_CLOCK
#define APBPeriph_AES_CLOCK               ((uint32_t)(  8 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_KEYSCAN_CLOCK           ((uint32_t)(  6 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_QDEC_CLOCK              ((uint32_t)(  4 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_I2C1_CLOCK              ((uint32_t)(  2 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_I2C0_CLOCK              ((uint32_t)(  0 | (0x03UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

/* 0x23C */
#define APBPeriph_CAN1_CLOCK              ((uint32_t)( 30 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART5_CLOCK             ((uint32_t)( 28 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART4_CLOCK             ((uint32_t)( 26 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART3_CLOCK             ((uint32_t)( 24 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART2_CLOCK             ((uint32_t)( 22 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART1_CLOCK             ((uint32_t)( 20 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_UART0_CLOCK             ((uint32_t)( 18 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_JPEG_CLOCK              ((uint32_t)( 16 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_ZIGBEE_CLOCK            ((uint32_t)( 14 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))
#define APBPeriph_CAN2_CLOCK              ((uint32_t)( 12 | (0x04UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

/* 0x244 */
#define APBPeriph_BTBUS_CLOCK             ((uint32_t)( 0 | (0x06UL << 29) | APBPeriph_SLEEP_CLOCK_EXIST))

#define IS_APB_PERIPH_CLOCK(CLOCK)  (((CLOCK) == APBPeriph_I2S2_CLOCK) || ((CLOCK) == APBPeriph_I2S1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_I2S0_CLOCK)|| ((CLOCK) == APBPeriph_CODEC_CLOCK) \
                                     || ((CLOCK) == APBPeriph_SD_HOST_CLOCK)|| ((CLOCK) == APBPeriph_GPIOA_CLOCK)\
                                     || ((CLOCK) == APBPeriph_GPIOB_CLOCK) || ((CLOCK) == APBPeriph_FLASH2_CLOCK)\
                                     || ((CLOCK) == APBPeriph_FLASH1_CLOCK) || ((CLOCK) == APBPeriph_DMA_CLOCK)\
                                     || ((CLOCK) == APBPeriph_TIMER_CLOCK) || ((CLOCK) == APBPeriph_FLASH_CLOCK)\
                                     || ((CLOCK) == APBPeriph_VENDOR_REG_CLOCK) || ((CLOCK) == APBPeriph_CKE_BTV_CLOCK)\
                                     || ((CLOCK) == APBPeriph_BUS_RAM_SLP_CLOCK) || ((CLOCK) == APBPeriph_CKE_CTRLAP_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CKE_PLFM_CLOCK)|| ((CLOCK) == APBPeriph_GPIO1_DEB_CLOCK)\
                                     || ((CLOCK) == APBPeriph_GPIO0_DEB_CLOCK) || ((CLOCK) == APBPeriph_RTZIP_CLOCK)\
                                     || ((CLOCK) == APBPeriph_PKE_CLOCK)|| ((CLOCK) == APBPeriph_PPE_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CAN0_CLOCK)||(CLOCK) == (APBPeriph_SPI2_CLOCK)\
                                     || ((CLOCK) ==APBPeriph_IR_CLOCK ) || (CLOCK) == (APBPeriph_SPI1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_SPI0_CLOCK)||((CLOCK) == APBPeriph_TIMER1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CKE_SM3_CLOCK)|| ((CLOCK) == APBPeriph_CKE_SHA256_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CKE_AAC_XTAL_CLOCK) || ((CLOCK) == APBPeriph_CKE_PDCK_CLOCK)\
                                     || ((CLOCK) == APBPeriph_RNG_CLOCK)||((CLOCK) == APBPeriph_SWR_SS_CLOCK)\
                                     || ((CLOCK) == APBPeriph_CAL_32K_CLOCK) || ((CLOCK) == APBPeriph_CKE_MODEM_CLOCK)\
                                     || ((CLOCK) == APBPeriph_SPI0_SLAVE_CLOCK) || ((CLOCK) == APBPeriph_EFUSE_CLOCK)\
                                     || ((CLOCK) == APBPeriph_DSP_MEM_CLOCK) || ((CLOCK) == APBPeriph_ASRC_CLOCK)\
                                     || ((CLOCK) == APBPeriph_DISP_CLOCK) || ((CLOCK) == APBPeriph_DATA_MEM1_CLOCK)\
                                     || ((CLOCK) == APBPeriph_DATA_MEM0_CLOCK) || ((CLOCK) == APBPeriph_CAN2_CLOCK)\
                                     || ((CLOCK) == APBPeriph_I2C2_CLOCK) || ((CLOCK) == APBPeriph_SIMC_CLOCK)\
                                     || ((CLOCK) == APBPeriph_AES_CLOCK)||((CLOCK) == APBPeriph_KEYSCAN_CLOCK)\
                                     || ((CLOCK) == APBPeriph_QDEC_CLOCK)||((CLOCK) == APBPeriph_I2C1_CLOCK )\
                                     || ((CLOCK) == APBPeriph_I2C0_CLOCK) || ((CLOCK) == APBPeriph_CAN1_CLOCK )\
                                     || ((CLOCK) == APBPeriph_UART5_CLOCK) || ((CLOCK) == APBPeriph_UART4_CLOCK )\
                                     || ((CLOCK) == APBPeriph_UART3_CLOCK) || ((CLOCK) == APBPeriph_UART2_CLOCK )\
                                     || ((CLOCK) == APBPeriph_UART1_CLOCK )|| ((CLOCK) == APBPeriph_UART0_CLOCK )\
                                     || ((CLOCK) == APBPeriph_JPEG_CLOCK )|| ((CLOCK) == APBPeriph_ZIGBEE_CLOCK )\
                                     || ((CLOCK) == APBPeriph_BTBUS_CLOCK) || ((CLOCK) == APBPeriph_GMAC_CLOCK)\
                                     || ((CLOCK) == APBPeriph_SD_HOST1_CLOCK) || ((CLOCK) == APBPeriph_FLASH3_CLOCK)\
                                     || ((CLOCK) == APBPeriph_AHBC_CLOCK) || ((CLOCK) == APBPeriph_2P4G_CLOCK)\
                                    )

/** End of group RCC_Peripheral_Clock
  * @}
  */

/** @defgroup APB_Peripheral_Define APB Peripheral Define
  * @{
  */
/* 0x210 */
#define APBPeriph_TIMER1_DEB            ((uint32_t)( 31 | (0x00UL << 26)))
#define APBPeriph_ZIGBEE                ((uint32_t)( 30 | (0x00UL << 26)))
#define APBPeriph_GMAC                  ((uint32_t)( 29 | (0x00UL << 26)))
#define APBPeriph_JPEG                  ((uint32_t)( 28 | (0x00UL << 26)))
#define APBPeriph_CAN1                  ((uint32_t)( 27 | (0x00UL << 26)))
#define APBPeriph_RTZIP                 ((uint32_t)( 26 | (0x00UL << 26)))
#define APBPeriph_PKE                   ((uint32_t)( 25 | (0x00UL << 26)))
#define APBPeriph_PPE                   ((uint32_t)( 24 | (0x00UL << 26)))
#define APBPeriph_CAN0                  ((uint32_t)( 23 | (0x00UL << 26)))
#define APBPeriph_TIMER1                ((uint32_t)( 22 | (0x00UL << 26)))
#define APBPeriph_AAC_XTAL              ((uint32_t)( 20 | (0x00UL << 26)))
#define APBPeriph_PDCK                  ((uint32_t)( 19 | (0x00UL << 26)))
#define APBPeriph_SWR_SS                ((uint32_t)( 18 | (0x00UL << 26)))
#define APBPeriph_CAN2                  ((uint32_t)( 17 | (0x00UL << 26)))
#define APBPeriph_TIMER                 ((uint32_t)( 16 | (0x00UL << 26)))
#define APBPeriph_TIMERA                APBPeriph_TIMER
#define APBPeriph_USB                   ((uint32_t)( 15 | (0x00UL << 26)))
#define APBPeriph_SD_HOST               ((uint32_t)( 14 | (0x00UL << 26)))
#define APBPeriph_DMA                  ((uint32_t)( 13 | (0x00UL << 26)))
#define APBPeriph_UART5                 ((uint32_t)( 12 | (0x00UL << 26)))
#define APBPeriph_UART4                 ((uint32_t)( 11 | (0x00UL << 26)))
#define APBPeriph_UART3                 ((uint32_t)( 10 | (0x00UL << 26)))
#define APBPeriph_UART2                 ((uint32_t)( 9  | (0x00UL << 26)))
#define APBPeriph_UART1                 ((uint32_t)( 8  | (0x00UL << 26)))
#define APBPeriph_UART0                 ((uint32_t)( 7  | (0x00UL << 26)))
#define APBPeriph_FLASH2                ((uint32_t)( 6  | (0x00UL << 26)))
#define APBPeriph_FLASH1                ((uint32_t)( 5  | (0x00UL << 26)))
#define APBPeriph_FLASH                 ((uint32_t)( 4  | (0x00UL << 26)))
#define APBPeriph_FLASH3                ((uint32_t)( 3  | (0x00UL << 26)))
#define APBPeriph_BTBUS                 ((uint32_t)( 2  | (0x00UL << 26)))
#define APBPeriph_SD_HOST1              ((uint32_t)( 1  | (0x00UL << 26)))
#define APBPeriph_2P4G                  ((uint32_t)( 0  | (0x00UL << 26)))

/* 0x218 */
#define APBPeriph_DATA_MEM              ((uint32_t)( 31 | (0x02UL << 26)))
#define APBPeriph_EFUSE                 ((uint32_t)( 30 | (0x02UL << 26)))
#define APBPeriph_DSP_WDT               ((uint32_t)( 29 | (0x02UL << 26)))
#define APBPeriph_ASRC                  ((uint32_t)( 28 | (0x02UL << 26)))
#define APBPeriph_DSP_MEM               ((uint32_t)( 27 | (0x02UL << 26)))
#define APBPeriph_DSP_H2D_D2H           ((uint32_t)( 26 | (0x02UL << 26)))
#define APBPeriph_DSP_CORE              ((uint32_t)( 25 | (0x02UL << 26)))
#define APBPeriph_SPI0_SLAVE            ((uint32_t)( 24 | (0x02UL << 26)))
#define APBPeriph_PSRAM3                ((uint32_t)( 23 | (0x02UL << 26)))
#define APBPeriph_PSRAM                 ((uint32_t)( 22 | (0x02UL << 26)))
#define APBPeriph_I2C2                  ((uint32_t)( 20 | (0x02UL << 26)))
#define APBPeriph_KEYSCAN               ((uint32_t)( 19 | (0x02UL << 26)))
#define APBPeriph_QDEC                  ((uint32_t)( 18 | (0x02UL << 26)))
#define APBPeriph_I2C1                  ((uint32_t)( 17 | (0x02UL << 26)))
#define APBPeriph_I2C0                  ((uint32_t)( 16 | (0x02UL << 26)))
#define APBPeriph_SPI2                  ((uint32_t)( 11 | (0x02UL << 26)))
#define APBPeriph_IR                    ((uint32_t)( 10 | (0x02UL << 26)))
#define APBPeriph_SPI1                  ((uint32_t)( 9  | (0x02UL << 26)))
#define APBPeriph_SPI0                  ((uint32_t)( 8  | (0x02UL << 26)))
#define APBPeriph_SM3                   ((uint32_t)( 7  | (0x02UL << 26)))
#define APBPeriph_SHA256                ((uint32_t)( 6  | (0x02UL << 26)))
#define APBPeriph_DISP                  ((uint32_t)( 5  | (0x02UL << 26)))
#define APBPeriph_SIMC                  ((uint32_t)( 4  | (0x02UL << 26)))
#define APBPeriph_ISO7816               APBPeriph_SIMC
#define APBPeriph_RNG                   ((uint32_t)( 3  | (0x02UL << 26)))
#define APBPeriph_AES                   ((uint32_t)( 2  | (0x02UL << 26)))

/* 0x21C */
#define APBPeriph_TIMER1_9_PWM          ((uint32_t)( 13  | (0x03UL << 26)))
#define APBPeriph_TIMER1_8_PWM          ((uint32_t)( 12  | (0x03UL << 26)))
#define APBPeriph_TIMER1_7_PWM          ((uint32_t)( 11  | (0x03UL << 26)))
#define APBPeriph_TIMER1_6_PWM          ((uint32_t)( 10  | (0x03UL << 26)))
#define APBPeriph_GPIOB                 ((uint32_t)( 9  | (0x03UL << 26)))
#define APBPeriph_GPIOA                 ((uint32_t)( 8  | (0x03UL << 26)))
#define APBPeriph_GPIO                   APBPeriph_GPIOA
#define APBPeriph_ADC                   ((uint32_t)(0  | (0x03UL << 26)))

/* 0x220 */
#define APBPeriph_I2S2                  ((uint32_t)((1 << 10) | (0x04UL << 26)))
#define APBPeriph_I2S1                  ((uint32_t)((1 << 2)  | (0x04UL << 26)))
#define APBPeriph_I2S0                  ((uint32_t)((1 << 1)  | (0x04UL << 26)))
#define APBPeriph_CODEC                 ((uint32_t)((1 << 0)  | (0x04UL << 26)))

/* No periph function bit */
#define APBPeriph_NO_FUNCTION_BIT       (0)
#define APBPeriph_CKE_MODEM             (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_VENDOR_REG            (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_CKE_BTV               (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_BUS_RAM_SLP           (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_CKE_CTRLAP            (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_CKE_PLFM              (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_GPIO1_DEB             (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_GPIO0_DEB             (APBPeriph_NO_FUNCTION_BIT)
#define APBPeriph_AHBC                  (APBPeriph_NO_FUNCTION_BIT)

#define IS_APB_PERIPH(PERIPH) (((PERIPH) == APBPeriph_TIMER1_DEB) || ((PERIPH) == APBPeriph_ZIGBEE)\
                               || ((PERIPH) == APBPeriph_GMAC)\
                               || ((PERIPH) == APBPeriph_JPEG) || ((PERIPH) == APBPeriph_CAN1)\
                               || ((PERIPH) == APBPeriph_RTZIP) || ((PERIPH) == APBPeriph_PKE)\
                               || ((PERIPH) == APBPeriph_PPE) || ((PERIPH) == APBPeriph_CAN0)\
                               || ((PERIPH) == APBPeriph_TIMER1)\
                               || ((PERIPH) == APBPeriph_AAC_XTAL)|| ((PERIPH) == APBPeriph_PDCK)\
                               || ((PERIPH) == APBPeriph_SWR_SS) || ((PERIPH) == APBPeriph_TIMER)\
                               || ((PERIPH) == APBPeriph_USB) || ((PERIPH) == APBPeriph_SD_HOST)\
                               || ((PERIPH) == APBPeriph_DMA) || ((PERIPH) == APBPeriph_UART5)\
                               || ((PERIPH) == APBPeriph_UART4) || ((PERIPH) == APBPeriph_UART3)\
                               || ((PERIPH) == APBPeriph_UART2) || ((PERIPH) == APBPeriph_UART1)\
                               || ((PERIPH) == APBPeriph_UART0) || ((PERIPH) == APBPeriph_FLASH2)\
                               || ((PERIPH) == APBPeriph_FLASH1) || ((PERIPH) == APBPeriph_FLASH)\
                               || ((PERIPH) == APBPeriph_BTBUS)|| ((PERIPH) == APBPeriph_DATA_MEM)\
                               || ((PERIPH) == APBPeriph_EFUSE) || ((PERIPH) == APBPeriph_DSP_WDT)\
                               || ((PERIPH) == APBPeriph_ASRC) || ((PERIPH) == APBPeriph_DSP_MEM)\
                               || ((PERIPH) == APBPeriph_DSP_H2D_D2H)\
                               || ((PERIPH) == APBPeriph_DSP_CORE) || ((PERIPH) == APBPeriph_SPI0_SLAVE)\
                               |  ((PERIPH) == APBPeriph_PSRAM) || ((PERIPH) == APBPeriph_I2C2)\
                               || ((PERIPH) == APBPeriph_KEYSCAN) || ((PERIPH) == APBPeriph_QDEC)\
                               || ((PERIPH) == APBPeriph_I2C1)|| ((PERIPH) == APBPeriph_I2C0)\
                               || ((PERIPH) == APBPeriph_SPI2) || ((PERIPH) == APBPeriph_IR)\
                               || (PERIPH == APBPeriph_SPI1) || (PERIPH == APBPeriph_SPI0)\
                               || (PERIPH == APBPeriph_SM3) || (PERIPH == APBPeriph_SHA256)\
                               || (PERIPH == APBPeriph_DISP) || (PERIPH == APBPeriph_SIMC)\
                               || (PERIPH == APBPeriph_RNG)|| (PERIPH == APBPeriph_AES) \
                               || (PERIPH == APBPeriph_GPIOB)|| (PERIPH == APBPeriph_GPIOA) \
                               || (PERIPH == APBPeriph_ADC)|| (PERIPH == APBPeriph_I2S2) \
                               || (PERIPH == APBPeriph_I2S1)|| (PERIPH == APBPeriph_I2S0) \
                               || (PERIPH == APBPeriph_CODEC)|| (PERIPH == APBPeriph_CKE_MODEM) \
                               || (PERIPH == APBPeriph_VENDOR_REG)|| (PERIPH == APBPeriph_CKE_BTV) \
                               || (PERIPH == APBPeriph_BUS_RAM_SLP)|| (PERIPH == APBPeriph_CKE_CTRLAP) \
                               || (PERIPH == APBPeriph_CKE_PLFM)|| (PERIPH == APBPeriph_GPIO1_DEB) \
                               || (PERIPH == APBPeriph_GPIO0_DEB) || (PERIPH == APBPeriph_CAN2) \
                               || (PERIPH == APBPeriph_FLASH3) || (PERIPH == APBPeriph_SD_HOST1)\
                               || (PERIPH == APBPeriph_2P4G) || (PERIPH == APBPeriph_PSRAM3)\
                               || (PERIPH == APBPeriph_TIMER1_6_PWM) || (PERIPH == APBPeriph_TIMER1_7_PWM)\
                               || (PERIPH == APBPeriph_TIMER1_8_PWM) || (PERIPH == APBPeriph_TIMER1_9_PWM)\
                               || (PERIPH == APBPeriph_AHBC))

/** End of group APB_Peripheral_Define
  * @}
  */

#define RCC_ClockCmd(APBPeriph, NewState)   RCC_PeriphClockCmd(APBPeriph, APBPeriph##_CLOCK, NewState)


#ifdef __cplusplus
}
#endif

#endif /* RTL_RCC_DEF_H */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
