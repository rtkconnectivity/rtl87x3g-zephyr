/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_rcc_def.h
* \brief    RCC Registers Structures Section
* \details
* \author   renee
* \date
* \version  v1.1
* *********************************************************************************************************
*/

#ifndef RTL_RCC_DEF_H
#define RTL_RCC_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

/** @defgroup Pin_Function_Number Pin Function Number
  * @{
  */

/*============================================================================*
 *                         RCC Features
 *============================================================================*/

#define RCC_SUPPORT_CLOCKGATECMD_API                   (1)
#define RCC_SUPPORT_AUTO_CLOCK                         (0)

/*============================================================================*
 *                         RCC Constants
 *============================================================================*/

/** @defgroup RCC_Peripheral_Clock  RCC Peripheral Clock
  * @{
  */
#define APBPeriph_CKE_COM_RAM_CLOCK         ((uint32_t)( 30 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_USB_CLOCK                 ((uint32_t)( 28 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_SD_HOST_CLOCK             ((uint32_t)( 26 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_GPIOA_CLOCK               ((uint32_t)( 24 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_GPIO_CLOCK                APBPeriph_GPIOA_CLOCK
#define APBPeriph_GPIOB_CLOCK               ((uint32_t)( 22 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_TIMERB_CLOCK              ((uint32_t)( 20 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_UART3_CLOCK               ((uint32_t)( 18 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_GDMA_CLOCK                ((uint32_t)( 16 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_TIMERA_CLOCK              ((uint32_t)( 14 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_UART1_CLOCK               ((uint32_t)( 12 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_UART2_CLOCK               ((uint32_t)( 10 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_FLASH_CLOCK               ((uint32_t)(  8 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_VENDOR_REG_CLOCK          ((uint32_t)(  6 | (0x01 << 29) | 0x00 << 10))
#define APBPeriph_CKE_BTV_CLOCK             ((uint32_t)(  5 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_BUS_RAM_SLP_CLOCK         ((uint32_t)(  4 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_EFC_CLOCK                 ((uint32_t)(  3 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_CKE_PLFM_CLOCK            ((uint32_t)(  2 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_CKE_CORDIC_CLOCK          ((uint32_t)(  1 | (0x01 << 29) | 0x01 << 10))
#define APBPeriph_HWSPI_CLOCK               ((uint32_t)(  0 | (0x01 << 29) | 0x01 << 10))


#define APBPeriph_CKE_COM2_RAM_CLOCK       ((uint32_t)( 30 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_FLASH3_CLOCK             ((uint32_t)( 28 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_FLASH2_CLOCK             ((uint32_t)( 26 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_FLASH1_CLOCK             ((uint32_t)( 24 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_SHA_CLOCK                ((uint32_t)( 22 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_IR_CLOCK                 ((uint32_t)( 20 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_SPI1_CLOCK               ((uint32_t)( 18 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_SPI0_CLOCK               ((uint32_t)( 16 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_SPI2_CLOCK               ((uint32_t)( 14 | (0x02 << 29) | 0x00 << 10))
#define APBPeriph_ASRC2_CLOCK              ((uint32_t)( 13 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_DSP2_WDT_CLOCK           ((uint32_t)( 12 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_DSP_SYNC_CLOCK           ((uint32_t)( 11 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_DSP_TIMER_CLOCK          ((uint32_t)( 10 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_PSRAM_CLOCK              ((uint32_t)(  9 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_RNG_CLOCK                ((uint32_t)(  8 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_RSA_CLOCK                ((uint32_t)(  7 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_SWR_SS_CLOCK             ((uint32_t)(  6 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_CAL_32K_CLOCK            ((uint32_t)(  5 | (0x02 << 29) | 0x01 << 10))
#define APBPeriph_CKE_MODEM_CLOCK          ((uint32_t)( BIT(4) ))
#define APBPeriph_UART1_HCI_CLOCK          ((uint32_t)( BIT(2)|BIT(3) ))
#define APBPeriph_UART0_CLOCK              ((uint32_t)(  0 | (0x02 << 29) | 0x00 << 10))

#define APBPeriph_EFUSE_CLOCK             ((uint32_t)( 31 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_CKE_DSP_WDT_CLOCK       ((uint32_t)( 30 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_CKE_DSP_CLOCK           ((uint32_t)( 28 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_CKE_H2D_D2H             ((uint32_t)( 26 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_ADC_CLOCK               ((uint32_t)( 24 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_DSP_MEM_CLOCK           ((uint32_t)( 22 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_ASRC_CLOCK              ((uint32_t)( 20 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_DSP2_CLOCK              ((uint32_t)( 19 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_DSP2_MEM_CLOCK          ((uint32_t)( 18 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_SPI2W_CLOCK             ((uint32_t)( 16 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_DSP_BUS_CLOCK           ((uint32_t)( 15 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_DSP_SYS_RAM_CLOCK       ((uint32_t)( 14 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_DSP_PERI_CLOCK          ((uint32_t)( 13 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_DSP_DMA_CLOCK           ((uint32_t)( 12 | (0x03 << 29) | 0x01 << 10))
#define APBPeriph_I2C2_CLOCK              ((uint32_t)( 10 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_AES_CLOCK               ((uint32_t)(  8 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_KEYSCAN_CLOCK           ((uint32_t)(  6 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_QDEC_CLOCK              ((uint32_t)(  4 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_I2C1_CLOCK              ((uint32_t)(  2 | (0x03 << 29) | 0x00 << 10))
#define APBPeriph_I2C0_CLOCK              ((uint32_t)(  0 | (0x03 << 29) | 0x00 << 10))

#define APBPeriph_APSRAM_CLOCK            ((uint32_t)(  2 | ((uint32_t)0x05 << 29) | 0x01 << 10))
#define APBPeriph_GPIOC_CLOCK             ((uint32_t)(  0 | ((uint32_t)0x05 << 29) | 0x00 << 10))

#define APBPeriph_UART_CLOCK                APBPeriph_UART0_CLOCK
#define APBPeriph_TIMER_CLOCK               APBPeriph_TIMERA_CLOCK
#define IS_APB_PERIPH_CLOCK(CLOCK) (((CLOCK) == APBPeriph_GPIOA_CLOCK) || ((CLOCK) == APBPeriph_GPIOB_CLOCK)\
                                    || ((CLOCK) == APBPeriph_GDMA_CLOCK)\
                                    || ((CLOCK) == APBPeriph_TIMERA_CLOCK) || ((CLOCK) == APBPeriph_TIMERB_CLOCK)\
                                    || ((CLOCK) == APBPeriph_IR_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SPI1_CLOCK) || ((CLOCK) == APBPeriph_SPI0_CLOCK)\
                                    || ((CLOCK) == APBPeriph_UART0_CLOCK) || ((CLOCK) == APBPeriph_ADC_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SPI2W_CLOCK) || ((CLOCK) == APBPeriph_KEYSCAN_CLOCK)\
                                    || ((CLOCK) == APBPeriph_QDEC_CLOCK) || ((CLOCK) == APBPeriph_I2C1_CLOCK)\
                                    || ((CLOCK) == APBPeriph_I2C0_CLOCK) || ((CLOCK) == APBPeriph_CODEC_CLOCK)\
                                    || ((CLOCK) == APBPeriph_UART1_CLOCK)|| ((CLOCK) == APBPeriph_UART2_CLOCK)\
                                    || ((CLOCK) == APBPeriph_UART3_CLOCK)||((CLOCK) == APBPeriph_FLASH3_CLOCK)\
                                    || ((CLOCK) == APBPeriph_FLASH2_CLOCK)||((CLOCK) == APBPeriph_EFUSE_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SPI2_CLOCK)||(CLOCK) == (APBPeriph_FLASH_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_COM_RAM_CLOCK)||((CLOCK) ==A PBPeriph_USB_CLOCK )\
                                    || ((CLOCK) == APBPeriph_SD_HOST_CLOCK)||((CLOCK) == APBPeriph_GPIO_CLOCK)\
                                    || ((CLOCK) == APBPeriph_VENDOR_REG_CLOCK)|| ((CLOCK) == APBPeriph_CKE_BTV_CLOCK)\
                                    || ((CLOCK) == APBPeriph_BUS_RAM_SLP_CLOCK)||((CLOCK) == APBPeriph_EFC_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_PLFM_CLOCK)||((CLOCK) == APBPeriph_CKE_CORDIC_CLOCK)\
                                    || ((CLOCK) == APBPeriph_HWSPI_CLOCK)||(CLOCK) == (APBPeriph_CKE_COM2_RAM_CLOCK)\
                                    || ((CLOCK) == APBPeriph_DSP2_WDT_CLOCK)||((CLOCK) ==A APBPeriph_DSP_SYNC_CLOCK )\
                                    || ((CLOCK) == APBPeriph_SHA_CLOCK)||((CLOCK) == APBPeriph_ASRC2_CLOCK)\
                                    || ((CLOCK) == APBPeriph_DSP_TIMER_CLOCK)||((CLOCK) ==A APBPeriph_PSRAM_CLOCK )\
                                    || ((CLOCK) == APBPeriph_RNG_CLOCK)||((CLOCK) == APBPeriph_RSA_CLOCK)\
                                    || ((CLOCK) == APBPeriph_SWR_SS_CLOCK)|| ((CLOCK) == APBPeriph_CAL_32K_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_MODEM_CLOCK)||((CLOCK) == APBPeriph_UART1_HCI_CLOCK)\
                                    || ((CLOCK) == APBPeriph_EFUSE_CLOCK)||((CLOCK) == APBPeriph_CKE_DSP_WDT_CLOCK)\
                                    || ((CLOCK) == APBPeriph_CKE_DSP_CLOCK)||(CLOCK) == (APBPeriph_CKE_H2D_D2H)\
                                    || ((CLOCK) == APBPeriph_DSP_MEM_CLOCK)||((CLOCK) ==A APBPeriph_ASRC_CLOCK )\
                                    || ((CLOCK) == APBPeriph_DSP2_CLOCK)||((CLOCK) == APBPeriph_DSP2_MEM_CLOCK)\
                                    || ((CLOCK) == APBPeriph_DSP_BUS_CLOCK)||((CLOCK) == APBPeriph_DSP_SYS_RAM_CLOCK)\
                                    || ((CLOCK) == APBPeriph_DSP_PERI_CLOCK)||(CLOCK) == (APBPeriph_DSP_DMA_CLOCK)\
                                    || ((CLOCK) == APBPeriph_I2C2_CLOCK)||((CLOCK) ==A APBPeriph_AES_CLOCK )\
                                    || ((CLOCK) == APBPeriph_GPIOC_CLOCK)||((CLOCK) == APBPeriph_APSRAM_CLOCK)\
                                   )

/** End of group RCC_Peripheral_Clock
  * @}
  */

/** @defgroup APB_Peripheral_Define APB Peripheral Define
  * @{
  */
#define APBPeriph_ASRC2                 ((uint32_t)( 19 | (0x00 << 26)))
#define APBPeriph_SWR_SS                ((uint32_t)( 18 | (0x00 << 26)))
#define APBPeriph_TIMERB                ((uint32_t)( 17 | (0x00 << 26)))
#define APBPeriph_TIMERA                ((uint32_t)( 16 | (0x00 << 26)))
#define APBPeriph_USB                   ((uint32_t)( 15 | (0x00 << 26)))
#define APBPeriph_SD_HOST               ((uint32_t)( 14 | (0x00 << 26)))
#define APBPeriph_GDMA                  ((uint32_t)( 13 | (0x00 << 26)))
#define APBPeriph_UART1                 ((uint32_t)( 12 | (0x00 << 26)))
#define APBPeriph_FLASH3                ((uint32_t)( 6  | (0x00 << 26)))
#define APBPeriph_FLASH2                ((uint32_t)( 5  | (0x00 << 26)))
#define APBPeriph_FLASH                 ((uint32_t)( 4  | (0x00 << 26)))
#define APBPeriph_FLASH1                ((uint32_t)( 3  | (0x00 << 26)))
#define APBPeriph_BTBUS                 ((uint32_t)( 2  | (0x00 << 26)))


#define APBPeriph_SHA                   ((uint32_t)( 31 | (0x02 << 26)))
#define APBPeriph_EFUSE                 ((uint32_t)( 30 | (0x02 << 26)))
#define APBPeriph_DSP_WDT               ((uint32_t)( 29 | (0x02 << 26)))
#define APBPeriph_ASRC                  ((uint32_t)( 28 | (0x02 << 26)))
#define APBPeriph_DSP_MEM               ((uint32_t)( 27 | (0x02 << 26)))
#define APBPeriph_DSP_H2D_D2H           ((uint32_t)( 26 | (0x02 << 26)))
#define APBPeriph_DSP_CORE              ((uint32_t)( 25 | (0x02 << 26)))
#define APBPeriph_SPI2W                 ((uint32_t)( 24 | (0x02 << 26)))
#define APBPeriph_RSA                   ((uint32_t)( 23 | (0x02 << 26)))
#define APBPeriph_PSRAM                 ((uint32_t)( 22 | (0x02 << 26)))
#define APBPeriph_DSP2_MEM              ((uint32_t)( 21 | (0x02 << 26)))
#define APBPeriph_DSP2_CORE             ((uint32_t)( 20 | (0x02 << 26)))
#define APBPeriph_KEYSCAN               ((uint32_t)( 19 | (0x02 << 26)))
#define APBPeriph_QDEC                  ((uint32_t)( 18 | (0x02 << 26)))
#define APBPeriph_I2C1                  ((uint32_t)( 17 | (0x02 << 26)))
#define APBPeriph_I2C0                  ((uint32_t)( 16 | (0x02 << 26)))
#define APBPeriph_I2C2                  ((uint32_t)( 15 | (0x02 << 26)))
#define APBPeriph_DSP_BUS               ((uint32_t)( 14 | (0x02 << 26)))
#define APBPeriph_DSP_SYS_RAM           ((uint32_t)( 13 | (0x02 << 26)))
#define APBPeriph_DSP_PERI              ((uint32_t)( 12 | (0x02 << 26)))
#define APBPeriph_DSP_DMA               ((uint32_t)( 11 | (0x02 << 26)))
#define APBPeriph_IR                    ((uint32_t)( 10 | (0x02 << 26)))
#define APBPeriph_SPI1                  ((uint32_t)( 9  | (0x02 << 26)))
#define APBPeriph_SPI0                  ((uint32_t)( 8  | (0x02 << 26)))
#define APBPeriph_SPI2                  ((uint32_t)( 7  | (0x02 << 26)))
#define APBPeriph_DSP_TIMER             ((uint32_t)( 6  | (0x02 << 26)))
#define APBPeriph_DSP2_WDT              ((uint32_t)( 5  | (0x02 << 26)))
#define APBPeriph_UART3                 ((uint32_t)( 4  | (0x02 << 26)))
#define APBPeriph_RNG                   ((uint32_t)( 3  | (0x02 << 26)))
#define APBPeriph_AES                   ((uint32_t)( 2  | (0x02 << 26)))
#define APBPeriph_UART2                 ((uint32_t)( 1  | (0x02 << 26)))
#define APBPeriph_UART0                 ((uint32_t)( 0  | (0x02 << 26)))
#define APBPeriph_UART1_HCI             (0)
#define APBPeriph_CKE_MODEM             (0)

#define APBPeriph_APSRAM                ((uint32_t)(11  | (0x03 << 26)))
#define APBPeriph_GPIOC                 ((uint32_t)(10  | (0x03 << 26)))
#define APBPeriph_GPIOB                 ((uint32_t)( 9  | (0x03 << 26)))
#define APBPeriph_GPIOA                 ((uint32_t)( 8  | (0x03 << 26)))
#define APBPeriph_GPIO                   APBPeriph_GPIOA
#define APBPeriph_ADC                   ((uint32_t)(0  | (0x03 << 26)))

#define APBPeriph_UART                    APBPeriph_UART0
#define APBPeriph_TIMER                   APBPeriph_TIMERA

#define IS_APB_PERIPH(PERIPH) (((PERIPH) == APBPeriph_TIMERA) || ((PERIPH) == APBPeriph_TIMERB)\
                               || ((PERIPH) == APBPeriph_GDMA)\
                               || ((PERIPH) == APBPeriph_SPI2W) || ((PERIPH) == APBPeriph_KEYSCAN)\
                               || ((PERIPH) == APBPeriph_QDEC) || ((PERIPH) == APBPeriph_I2C1)\
                               || ((PERIPH) == APBPeriph_I2C0) || ((PERIPH) == APBPeriph_IR)\
                               || ((PERIPH) == APBPeriph_SPI1) || ((PERIPH) == APBPeriph_SPI0)\
                               || ((PERIPH) == APBPeriph_GPIOA)|| ((PERIPH) == APBPeriph_GPIOB)\
                               || ((PERIPH) == APBPeriph_ADC) || ((PERIPH) == APBPeriph_CODEC)\
                               || ((PERIPH) == APBPeriph_UART1) || (PERIPH == APBPeriph_UART0)\
                               || (PERIPH == APBPeriph_UART2) || (PERIPH == APBPeriph_UART3)\
                               || ((PERIPH) == APBPeriph_ASRC2) || ((PERIPH) == APBPeriph_KEYSCAN)\
                               || ((PERIPH) == APBPeriph_SWR_SS) || ((PERIPH) == APBPeriph_USB)\
                               || ((PERIPH) == APBPeriph_SD_HOST) || ((PERIPH) == APBPeriph_FLASH3)\
                               || ((PERIPH) == APBPeriph_FLASH2) || ((PERIPH) == APBPeriph_FLASH)\
                               || ((PERIPH) == APBPeriph_FLASH1)|| ((PERIPH) == APBPeriph_BTBUS)\
                               || ((PERIPH) == APBPeriph_SHA) || ((PERIPH) == APBPeriph_EFUSE)\
                               || ((PERIPH) == APBPeriph_DSP_MEM) || (PERIPH == APBPeriph_DSP_H2D_D2H)\
                               || (PERIPH == APBPeriph_DSP_CORE) || (PERIPH == APBPeriph_RSA)\
                               | ((PERIPH) == APBPeriph_PSRAM) || ((PERIPH) == APBPeriph_DSP2_MEM)\
                               || ((PERIPH) == APBPeriph_DSP2_CORE) || ((PERIPH) == APBPeriph_I2C2)\
                               || ((PERIPH) == APBPeriph_DSP_BUS)|| ((PERIPH) == APBPeriph_DSP_SYS_RAM)\
                               || ((PERIPH) == APBPeriph_DSP_PERI) || ((PERIPH) == APBPeriph_DSP_DMA)\
                               || ((PERIPH) == APBPeriph_IR) || (PERIPH == APBPeriph_SPI2)\
                               || (PERIPH == APBPeriph_DSP_TIMER) || (PERIPH == APBPeriph_DSP2_WDT)\
                               || (PERIPH == APBPeriph_RNG) || (PERIPH == APBPeriph_AES)\
                               || (PERIPH == APBPeriph_GPIOC) || (PERIPH == APBPeriph_APSRAM)\
                               || (PERIPH == APBPeriph_UART1_HCI) || (PERIPH == APBPeriph_CKE_MODEM)\
                              )

/** End of group APB_Peripheral_Define
  * @}
  */

/** @defgroup RCC_Clock_Source_Gating  RCC Clock Source Gating
  * @{
  */

#define CLOCK_GATE_5M                          ((uint32_t)(0x01 << 29))/* 5M clock source for adc and keyscan */
#define CLOCK_GATE_20M                         ((uint32_t)(0x01 << 27))/* 20M clock source for 2wssi and qdec */
#define CLOCK_GATE_10M                         ((uint32_t)(0x01 << 28))/* 10M clock source for bluewiz */
#define IS_CLOCK_GATE(CLOCK) (((CLOCK) == CLOCK_GATE_5M) || ((CLOCK) == CLOCK_GATE_20M)\
                              || ((CLOCK) == CLOCK_GATE_10M))

/** End of group RCC_Clock_Source_Gating
  * @}
  */


/** @defgroup RCC_TIM_Clock_Source_config RCC TIM Clock Source config
  * @{
  */

#define TIM_CLOCK_SOURCE_SYSTEM_CLOCK                              ((uint16_t) BIT7)
#define TIM_CLOCK_SOURCE_40MHZ                                     ((uint16_t)(BIT5 | BIT7))
#define TIM_CLOCK_SOURCE_PLL                                       ((uint16_t)(BIT6 | BIT7))
#define IS_TIM_CLOCK_SOURCE(HZ) (((HZ) == TIM_CLOCK_SOURCE_SYSTEM_CLOCK) || \
                                 ((HZ) == TIM_CLOCK_SOURCE_40MHZ) || ((HZ) == TIM_CLOCK_SOURCE_PLL))
/** End of group RCC_TIM_Clock_Source_config
  * @}
  */
/** @defgroup TIM_DIV TIM2TO7 Clock DIV
  * @{
  */

#define TIM2TO7_CLOCK_DIV_2                    ((uint16_t)0x1)
#define TIM2TO7_CLOCK_DIV_3                    ((uint16_t)0x2)
#define TIM2TO7_CLOCK_DIV_4                    ((uint16_t)0x3)
#define TIM2TO7_CLOCK_DIV_6                    ((uint16_t)0x4)
#define TIM2TO7_CLOCK_DIV_8                    ((uint16_t)0x5)
#define TIM2TO7_CLOCK_DIV_16                   ((uint16_t)0x6)
#define TIM2TO7_CLOCK_DIV_1                    ((uint16_t)0x8)
#define TIM2TO7_TIM_DIV(DIV)              (((DIV) == TIM2TO7_CLOCK_DIV_1) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_3) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_4) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_8) || \
                                           ((DIV) == TIM2TO7_CLOCK_DIV_16))
/** End of group TIM_DIV
* @}
*/

#define TIM_CLOCK_DIV_1                    ((uint16_t)0x0)
#define TIM_CLOCK_DIV_2                    ((uint16_t)0x4)
#define TIM_CLOCK_DIV_4                    ((uint16_t)0x5)
#define TIM_CLOCK_DIV_8                    ((uint16_t)0x6)
#define TIM_CLOCK_DIV_FIX_1MHZ             ((uint16_t)0x7)

#define TIM0TO11_TIM_DIV(DIV)              (((DIV) == TIM_CLOCK_DIV_1) || \
                                            ((DIV) == TIM_CLOCK_DIV_2) || \
                                            ((DIV) == TIM_CLOCK_DIV_4) || \
                                            ((DIV) == TIM_CLOCK_DIV_8) || \
                                            ((DIV) == TIM_CLOCK_DIV_FIX_1MHZ))

/** @defgroup SPI_clock_source SPI clock source
 * @{
 */

#define SPI_CLOCK_SOURCE_PLL                ((uint32_t)0x1)
#define SPI_CLOCK_SOURCE_40M                ((uint32_t)0x0)
#define IS_SPI_SOR(SOR)              (((SOR) == SPI_CLOCK_SOURCE_PLL) || \
                                      ((SOR) == SPI_CLOCK_SOURCE_40M))
/** End of group SPI_clock_source
  * @}
  */

/** @defgroup SPDIF_clock_divider SPDIF clock divider
  * @{
  */

#define SPDIF_CLOCK_SEC_DIV_DISABLE        ((uint16_t)0x0)
#define SPDIF_CLOCK_SEC_DIV_2              ((uint16_t)0x1)
#define IS_SPDIF_SEC_DIV(DIV)        (((DIV) == SPDIF_CLOCK_SEC_DIV_DISABLE) || \
                                      ((DIV) == SPDIF_CLOCK_SEC_DIV_2))
/** End of group SPDIF_clock_divider
  * @}
  */

/** @defgroup SPDIF_clock_source SPDIF clock source
 * @{
 */
#define SPDIF_CLOCK_SOURCE_40M             ((uint32_t)0x0)
#define SPDIF_CLOCK_SOURCE_PLL             ((uint32_t)0x1)
#define SPDIF_CLOCK_SOURCE_PLL2            ((uint32_t)0x2)
#define SPDIF_CLOCK_SOURCE_PLL3            ((uint32_t)0x3)
#define SPDIF_CLOCK_SOURCE_MCLK            ((uint32_t)0x4)

#define IS_SPDIF_SOR(SOR)            (((SOR) == SPDIF_CLOCK_SOURCE_40M) || \
                                      ((SOR) == SPDIF_CLOCK_SOURCE_PLL) || \
                                      ((SOR) == SPDIF_CLOCK_SOURCE_PLL2) || \
                                      ((SOR) == SPDIF_CLOCK_SOURCE_PLL3) || \
                                      ((SOR) == SPDIF_CLOCK_SOURCE_MCLK))
/** End of group SPDIF_clock_source
  * @}
  */

/*============================================================================*
 *                         RCC Wrapper
 *============================================================================*/

/**
 * \brief    RCC Macro Wrapper
 *
 * \ingroup  RCC_Exported_Constants
 */

/** @defgroup SPI_clock_divider SPI clock divider
  * @{
  */

#define SPI_CLOCK_DIV_1                    CLOCK_DIV_1
#define SPI_CLOCK_DIV_2                    CLOCK_DIV_2
#define SPI_CLOCK_DIV_4                    CLOCK_DIV_4
#define SPI_CLOCK_DIV_8                    CLOCK_DIV_8

/** End of group SPI_clock_divider
  * @}
  */

/** @defgroup I2C_clock_divider I2C clock divider
  * @{
  */

#define I2C_CLOCK_DIV_1                    CLOCK_DIV_1
#define I2C_CLOCK_DIV_2                    CLOCK_DIV_2
#define I2C_CLOCK_DIV_4                    CLOCK_DIV_4
#define I2C_CLOCK_DIV_8                    CLOCK_DIV_8

/** End of group I2C_clock_divider
  * @}
  */

/** @defgroup UART_clock_divider UART clock divider
  * @{
  */

#define UART_CLOCK_DIV_1                    ((uint16_t)0x0)
#define UART_CLOCK_DIV_2                    ((uint16_t)0x1)
#define UART_CLOCK_DIV_4                    ((uint16_t)0x2)
#define UART_CLOCK_DIV_16                   ((uint16_t)0x3)

/** End of group UART_clock_divider
  * @}
  */

#define RCC_ClockCmd(APBPeriph, NewState)   RCC_PeriphClockCmd(APBPeriph, APBPeriph##_CLOCK, NewState)


#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_RCC_DEF_H */
