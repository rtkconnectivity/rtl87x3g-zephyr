/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_rcc_def.h
* \brief    RCC Structures Section and parameter definition for RTL8772F
* \details
* \author   renee
* \date     2023-11-15
* \version  v1.1
* *********************************************************************************************************
*/

#ifndef RTL_RCC_DEF_H
#define RTL_RCC_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                         RCC Features
 *============================================================================*/

#define RCC_SUPPORT_CLOCKGATECMD_API                   (0)
#define RCC_SUPPORT_AUTO_CLOCK                         (0)

/*============================================================================*
 *                         RCC Constants
 *============================================================================*/

/**
 * \brief       RCC Peripheral Clock
 *
 * \ingroup     RCC_Exported_Constants
 */

#define APBPeriph_SPIC0_CLOCK               ((uint32_t)(1 << 30))//new 220107
#define APBPeriph_SPIC1_CLOCK               ((uint32_t)(1 << 30))//new 220107
#define APBPeriph_SPIC2_CLOCK               ((uint32_t)(1 << 30))//new 220107
#define APBPeriph_SPIC3_CLOCK               ((uint32_t)(1 << 30))//new 220107

#define APBPeriph_GDMA2_CLOCK               ((uint32_t)(1 << 3))//new 220107
#define APBPeriph_GDMA1_CLOCK               ((uint32_t)(1 << 1))//new 220107

#define APBPeriph_SPI1_CLOCK                ((uint32_t)(1 << 24))//new 220107
#define APBPeriph_SPI0_SLAVE_CLOCK          ((uint32_t)(1 << 19))
#define APBPeriph_I2C2_CLOCK                ((uint32_t)(1 << 13))
#define APBPeriph_I2C1_CLOCK                ((uint32_t)(1 << 7))
#define APBPeriph_I2C0_CLOCK                ((uint32_t)(1 << 1))

#define APBPeriph_SPI0_CLOCK                ((uint32_t)(1 << 9))//new 220107
#define APBPeriph_ADC_CLOCK                 ((uint32_t)(1 << 1))

#define APBPeriph_UART5_CLOCK               ((uint32_t)(1 << 23))//new 220107
#define APBPeriph_UART4_CLOCK               ((uint32_t)(1 << 16))
#define APBPeriph_UART3_CLOCK               ((uint32_t)(1 << 9))
#define APBPeriph_UART2_CLOCK               ((uint32_t)(1 << 2))

#define APBPeriph_TIMERB_CLOCK              ((uint32_t)(1 << 31))//new 220107
#define APBPeriph_TIMERB_PWM_WRAP1_CLOCK    ((uint32_t)(1 << 13))

#define APBPeriph_TIMERB_PWM_WRAP2_CLOCK    ((uint32_t)(1 << 13))//new 220107

#define APBPeriph_TIMERB_PWM_WRAP3_CLOCK    ((uint32_t)(1 << 13))//new 220107

#define APBPeriph_TIMERB_PWM_WRAP4_CLOCK    ((uint32_t)(1 << 13))//new 220107

#define APBPeriph_ENHTIMER_CLOCK            ((uint32_t)(1 << 31))//new 220107
#define APBPeriph_ENHTIMER_PWM1_CLOCK       ((uint32_t)(1 << 29))
#define APBPeriph_ENHTIMER_PWM0_CLOCK       ((uint32_t)(1 << 13))

#define APBPeriph_ENHTIMER_PWM3_CLOCK       ((uint32_t)(1 << 29))//new 220107
#define APBPeriph_ENHTIMER_PWM2_CLOCK       ((uint32_t)(1 << 13))

#define APBPeriph_ENHTIMER_PWM5_CLOCK       ((uint32_t)(1 << 29))//new 220107
#define APBPeriph_ENHTIMER_PWM4_CLOCK       ((uint32_t)(1 << 13))

#define APBPeriph_ENHTIMER_PWM7_CLOCK       ((uint32_t)(1 << 29))//new 220107
#define APBPeriph_ENHTIMER_PWM6_CLOCK       ((uint32_t)(1 << 13))

#define APBPeriph_GPIOD_CLOCK               ((uint32_t)(1 << 11))//new 220107
#define APBPeriph_GPIOC_CLOCK               ((uint32_t)(1 << 9))
#define APBPeriph_GPIOB_CLOCK               ((uint32_t)(1 << 7))
#define APBPeriph_GPIOA_CLOCK               ((uint32_t)(1 << 5))
#define APBPeriph_KEYSCAN_CLOCK             ((uint32_t)(1 << 3))
#define APBPeriph_QDEC_CLOCK                ((uint32_t)(1 << 1))

#define APBPeriph_IR_CLOCK                  ((uint32_t)(1 << 19))//new 220107
#define APBPeriph_I2C3_CLOCK                ((uint32_t)(1 << 14))
#define APBPeriph_ISO7816_CLOCK             ((uint32_t)(1 << 9))
#define APBPeriph_UART6_CLOCK               ((uint32_t)(1 << 4))

#define APBPeriph_SPI2_HS_CLOCK             ((uint32_t)(1 << 20))//new 220107
#define APBPeriph_SPI2_CLOCK                ((uint32_t)(1 << 12)|(1 << 18))
#define APBPeriph_SPI3_HS_CLOCK             ((uint32_t)(1 << 8))
#define APBPeriph_SPI3_CLOCK                ((uint32_t)(1 << 0)|(1 << 6))

#define APBPeriph_TIMERC_CLOCK              ((uint32_t)(1 << 31))//new 220107

#define APBPeriph_CAN_CLOCK                 ((uint32_t)(1 << 28))//new 220107
#define APBPeriph_DISP_CLOCK_CLOCK          ((uint32_t)(1 << 23))//new 220107
#define APBPeriph_MIPI_HOST_CLOCK           ((uint32_t)(1 << 8))//new 220107
#define APBPeriph_GPU_CLOCK_CLOCK           ((uint32_t)(1 << 0))//new 220107

#define APBPeriph_SDIO0_CLOCK               ((uint32_t)(1 << 16))//new 220107
#define APBPeriph_SDIO1_CLOCK               ((uint32_t)(1 << 0))//new 220107

#define APBPeriph_PKE_CLOCK                 ((uint32_t)(1 << 24))//new 220107
#define APBPeriph_AES_CLOCK                 ((uint32_t)(1 << 22))//new 220107
#define APBPeriph_SHA2_CLOCK                ((uint32_t)(1 << 20))//new 220107
#define APBPeriph_ECC_CLOCK                 ((uint32_t)(1 << 18))//new 220107
#define APBPeriph_SHA3_CLOCK                ((uint32_t)(1 << 14))//new 220107
#define APBPeriph_RNG_CLOCK                 ((uint32_t)(1 << 1))//new 220107

#define APBPeriph_GMAC_CLOCK                ((uint32_t)(1 << 13))//new 230512

#define APBPeriph_CODEC_CLOCK               ((uint32_t)(1 << 9))//new vcore3 221223
#define APBPeriph_I2S3_CLOCK                ((uint32_t)(1 << 7))
#define APBPeriph_I2S2_CLOCK                ((uint32_t)(1 << 5))
#define APBPeriph_I2S1_CLOCK                ((uint32_t)(1 << 3))
#define APBPeriph_I2S0_CLOCK                ((uint32_t)(1 << 1))

#define APBPeriph_USB_CLOCK                 ((uint32_t)(1 << 28))//new vcore3 221223

#define IS_APB_PERIPH_CLOCK(CLOCK) (((CLOCK) == APBPeriph_GDMA2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPI1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GDMA1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPI0_SLAVE_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2C2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2C1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2C0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPI0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ADC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_UART5_CLOCK) \
                                    || ((CLOCK) == APBPeriph_UART4_CLOCK) \
                                    || ((CLOCK) == APBPeriph_UART3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_UART2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_TIMERB_CLOCK) \
                                    || ((CLOCK) == APBPeriph_TIMERB_PWM_WRAP1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_TIMERB_PWM_WRAP2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_TIMERB_PWM_WRAP3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_TIMERB_PWM_WRAP4_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM5_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM4_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM7_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ENHTIMER_PWM6_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GPIOD_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GPIOC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GPIOB_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GPIOA_CLOCK) \
                                    || ((CLOCK) == APBPeriph_KEYSCAN_CLOCK) \
                                    || ((CLOCK) == APBPeriph_QDEC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_IR_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2C3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ISO7816_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SDIC0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SDIC1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SDIC2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SDIC3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SDIO0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SDIO1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_USB_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPI2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPI3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_TIMERC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_UART6_CLOCK) \
                                    || ((CLOCK) == APBPeriph_CODEC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2S0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2S1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2S2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_I2S3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPIC0_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPIC1_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPIC2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SPIC3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_DISP_CLOCK_CLOCK) \
                                    || ((CLOCK) == APBPeriph_MIPI_HOST_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GPU_CLOCK_CLOCK) \
                                    || ((CLOCK) == APBPeriph_PKE_CLOCK) \
                                    || ((CLOCK) == APBPeriph_AES_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SHA2_CLOCK) \
                                    || ((CLOCK) == APBPeriph_ECC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_SHA3_CLOCK) \
                                    || ((CLOCK) == APBPeriph_RNG_CLOCK) \
                                    || ((CLOCK) == APBPeriph_GMAC_CLOCK) \
                                    || ((CLOCK) == APBPeriph_CAN_CLOCK) )

/**
 * \brief       APB Peripheral Define
 *
 * \ingroup     RCC_Exported_Constants
 */

#define APBPeriph_SPIC0                 ((uint32_t)(0x800))//0x48//0x00 new 220107
#define APBPeriph_SPIC1                 ((uint32_t)(0x805))//0x48//0x14 new 220107
#define APBPeriph_SPIC2                 ((uint32_t)(0x80A))//0x48//0x28 new 220107
#define APBPeriph_SPIC3                 ((uint32_t)(0x80F))//0x48//0x3C new 220107

#define APBPeriph_GDMA2                 ((uint32_t)(0x14))//0x50 new 220107
#define APBPeriph_GDMA1                 ((uint32_t)(0x14))

#define APBPeriph_SPI1                  ((uint32_t)(0x15))//0x54 new 220107
#define APBPeriph_SPI0_SLAVE            ((uint32_t)(0x15))
#define APBPeriph_I2C2                  ((uint32_t)(0x15))
#define APBPeriph_I2C1                  ((uint32_t)(0x15))
#define APBPeriph_I2C0                  ((uint32_t)(0x15))

#define APBPeriph_SPI0                  ((uint32_t)(0x16))//0x58 new 220107
#define APBPeriph_ADC                   ((uint32_t)(0x16))

#define APBPeriph_UART5                 ((uint32_t)(0x17))//0x5C new 220107
#define APBPeriph_UART4                 ((uint32_t)(0x17))
#define APBPeriph_UART3                 ((uint32_t)(0x17))
#define APBPeriph_UART2                 ((uint32_t)(0x17))

#define APBPeriph_TIMERB                ((uint32_t)(0x18))//0x18//0x60 new 220107
#define APBPeriph_TIMERB_PWM_WRAP1      ((uint32_t)(0x18))

#define APBPeriph_TIMERB_PWM_WRAP2      ((uint32_t)(0x19))//0x1c//0x64 new 220107

#define APBPeriph_TIMERB_PWM_WRAP3      ((uint32_t)(0x1A))//0x20//0x68 new 220107

#define APBPeriph_TIMERB_PWM_WRAP4      ((uint32_t)(0x1B))//0x24//0x6C new 220107

#define APBPeriph_ENHTIMER              ((uint32_t)(0x1D))//0x28//0x74 new 220107
#define APBPeriph_ENHTIMER_PWM1         ((uint32_t)(0x1D))
#define APBPeriph_ENHTIMER_PWM0         ((uint32_t)(0x1D))

#define APBPeriph_ENHTIMER_PWM3         ((uint32_t)(0x1E))//0x2c//0x78 new 220107
#define APBPeriph_ENHTIMER_PWM2         ((uint32_t)(0x1E))

#define APBPeriph_ENHTIMER_PWM5         ((uint32_t)(0x1F))//0x30//0x7C new 220107
#define APBPeriph_ENHTIMER_PWM4         ((uint32_t)(0x1F))

#define APBPeriph_ENHTIMER_PWM7         ((uint32_t)(0x20))//0x34//0x80 new 220107
#define APBPeriph_ENHTIMER_PWM6         ((uint32_t)(0x20))

#define APBPeriph_GPIOD                 ((uint32_t)(0x21))//0x38//0x84 new 220107
#define APBPeriph_GPIOC                 ((uint32_t)(0x21))
#define APBPeriph_GPIOB                 ((uint32_t)(0x21))
#define APBPeriph_GPIOA                 ((uint32_t)(0x21))
#define APBPeriph_KEYSCAN               ((uint32_t)(0x21))
#define APBPeriph_QDEC                  ((uint32_t)(0x21))

#define APBPeriph_IR                    ((uint32_t)(0x836))//0x44//0xD8 new 220107
#define APBPeriph_I2C3                  ((uint32_t)(0x836))
#define APBPeriph_ISO7816               ((uint32_t)(0x836))//0x48//0xD8 new 220107
#define APBPeriph_UART6                 ((uint32_t)(0x836))//0x50//0xD8 new 220107

//#define APBPeriph_HS_SPI2               ((uint32_t)(0x37))//0x48//0xDC new 220107
#define APBPeriph_SPI2                  ((uint32_t)(0x837))//0x48//0xDC new 220107
//#define APBPeriph_HS_SPI3               ((uint32_t)(0x37))//0x48//0xDC new 220107
#define APBPeriph_SPI3                  ((uint32_t)(0x837))//0x48//0xDC new 220107

#define APBPeriph_TIMERC                ((uint32_t)(0x38))//0x4c//0xE0 new 220107

#define APBPeriph_CAN                   ((uint32_t)(0x839))//0xE4 new 230512 add
#define APBPeriph_DISP                  ((uint32_t)(0x839))//0xE4 new 220107 add
#define APBPeriph_MIPI_HOST             ((uint32_t)(0x839))//0xE4 new 220107 add

#define APBPeriph_GPU                   ((uint32_t)(0xA39))//0xE4 new 220107 add

#define APBPeriph_SDIO0                 ((uint32_t)(0x83A))//0x48//0xE8 new 220107
#define APBPeriph_SDIO1                 ((uint32_t)(0x83A))//0x48//0xE8 new 220107

#define APBPeriph_PKE                   ((uint32_t)(0x850))//0x140 new 220107 add
#define APBPeriph_AES                   ((uint32_t)(0x850))//0x140 new 220107 add
#define APBPeriph_SHA2                  ((uint32_t)(0x850))//0x140 new 220107 add
#define APBPeriph_ECC                   ((uint32_t)(0x850))//0x140 new 220107 add
#define APBPeriph_SHA3                  ((uint32_t)(0x850))//0x140 new 220107 add
#define APBPeriph_RNG                   ((uint32_t)(0x50))//0x140 new 220107 add

#define APBPeriph_GMAC                  ((uint32_t)(0x864))//0x290 new 230512 add

#define APBPeriph_CODEC                 ((uint32_t)(0x100))//0x50//0x9_00 new 211223 in vcore3
#define APBPeriph_I2S3                  ((uint32_t)(0x100))//0x50//0x9_00 new 211223 in vcore3
#define APBPeriph_I2S2                  ((uint32_t)(0x100))//0x50//0x9_00 new 211223 in vcore3
#define APBPeriph_I2S1                  ((uint32_t)(0x100))//0x50//0x9_00 new 211223 in vcore3
#define APBPeriph_I2S0                  ((uint32_t)(0x100))//0x50//0x9_00 new 211223 in vcore3

#define APBPeriph_USB                   ((uint32_t)(0x103))//0x48//0x9_0C new 211223 in vcore3

#define IS_APB_PERIPH(PERIPH) (((PERIPH) == APBPeriph_GDMA2) \
                               || ((PERIPH) == APBPeriph_SPI1) \
                               || ((PERIPH) == APBPeriph_GDMA1) \
                               || ((PERIPH) == APBPeriph_SPI0_SLAVE) \
                               || ((PERIPH) == APBPeriph_I2C2) \
                               || ((PERIPH) == APBPeriph_I2C1) \
                               || ((PERIPH) == APBPeriph_I2C0) \
                               || ((PERIPH) == APBPeriph_SPI0) \
                               || ((PERIPH) == APBPeriph_ADC) \
                               || ((PERIPH) == APBPeriph_UART5) \
                               || ((PERIPH) == APBPeriph_UART4) \
                               || ((PERIPH) == APBPeriph_UART3) \
                               || ((PERIPH) == APBPeriph_UART2) \
                               || ((PERIPH) == APBPeriph_TIMERB) \
                               || ((PERIPH) == APBPeriph_TIMERB_PWM_WRAP1) \
                               || ((PERIPH) == APBPeriph_TIMERB_PWM_WRAP2) \
                               || ((PERIPH) == APBPeriph_TIMERB_PWM_WRAP3) \
                               || ((PERIPH) == APBPeriph_TIMERB_PWM_WRAP4) \
                               || ((PERIPH) == APBPeriph_ENHTIMER) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM1) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM0) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM3) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM2) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM5) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM4) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM7) \
                               || ((PERIPH) == APBPeriph_ENHTIMER_PWM6) \
                               || ((PERIPH) == APBPeriph_GPIOD) \
                               || ((PERIPH) == APBPeriph_GPIOC) \
                               || ((PERIPH) == APBPeriph_GPIOB) \
                               || ((PERIPH) == APBPeriph_GPIOA) \
                               || ((PERIPH) == APBPeriph_KEYSCAN) \
                               || ((PERIPH) == APBPeriph_QDEC) \
                               || ((PERIPH) == APBPeriph_IR) \
                               || ((PERIPH) == APBPeriph_I2C3) \
                               || ((PERIPH) == APBPeriph_ISO7816) \
                               || ((PERIPH) == APBPeriph_SDIC0) \
                               || ((PERIPH) == APBPeriph_SDIC1) \
                               || ((PERIPH) == APBPeriph_SDIC2) \
                               || ((PERIPH) == APBPeriph_SDIC3) \
                               || ((PERIPH) == APBPeriph_SDIO0) \
                               || ((PERIPH) == APBPeriph_SDIO1) \
                               || ((PERIPH) == APBPeriph_USB) \
                               || ((PERIPH) == APBPeriph_SPI2) \
                               || ((PERIPH) == APBPeriph_SPI3) \
                               || ((PERIPH) == APBPeriph_TIMERC) \
                               || ((PERIPH) == APBPeriph_UART6) \
                               || ((PERIPH) == APBPeriph_CODEC) \
                               || ((PERIPH) == APBPeriph_I2S0) \
                               || ((PERIPH) == APBPeriph_I2S1) \
                               || ((PERIPH) == APBPeriph_I2S2) \
                               || ((PERIPH) == APBPeriph_I2S3) \
                               || ((PERIPH) == APBPeriph_SPIC0) \
                               || ((PERIPH) == APBPeriph_SPIC1) \
                               || ((PERIPH) == APBPeriph_SPIC2) \
                               || ((PERIPH) == APBPeriph_SPIC3) \
                               || ((PERIPH) == APBPeriph_DISP) \
                               || ((PERIPH) == APBPeriph_MIPI_HOST) \
                               || ((PERIPH) == APBPeriph_GPU) \
                               || ((PERIPH) == APBPeriph_PKE) \
                               || ((PERIPH) == APBPeriph_AES) \
                               || ((PERIPH) == APBPeriph_SHA2) \
                               || ((PERIPH) == APBPeriph_ECC) \
                               || ((PERIPH) == APBPeriph_SHA3) \
                               || ((PERIPH) == APBPeriph_RNG) \
                               || ((PERIPH) == APBPeriph_GMAC) \
                               || ((PERIPH) == APBPeriph_CAN))

#define RCC_ClockCmd(APBPeriph, NewState)   RCC_PeriphClockCmd(APBPeriph, APBPeriph##_CLOCK, NewState)


#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_RCC_DEF_H */
