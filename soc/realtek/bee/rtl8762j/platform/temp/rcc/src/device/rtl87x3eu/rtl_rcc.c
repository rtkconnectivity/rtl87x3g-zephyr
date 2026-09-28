
/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl_rcc.c
* @brief    This file provides all the IO clock firmware functions..
* @details
* @author   colin
* @date     2024-06-14
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl_rcc.h"
#include "platform_utils.h"
#include "pcc_reg.h"

#define IS_5M_PERI_CLK(CLOCK)     (((CLOCK) == APBPeriph_KEYSCAN_CLOCK) || ((CLOCK) == APBPeriph_SIMC_CLOCK))
#define IS_10M_PERI_CLK(CLOCK)    ((CLOCK) == APBPeriph_ADC_CLOCK)
#define IS_20M_PERI_CLK(CLOCK)    (((CLOCK) == APBPeriph_QDEC_CLOCK))
#define IS_TIMER_CLK(CLOCK)       (((CLOCK) == APBPeriph_TIMER_CLOCK) || (CLOCK == APBPeriph_TIMER1_CLOCK))

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMER1_DEB
  *     @arg APBPeriph_ZIGBEE
  *     @arg APBPeriph_GMAC
  *     @arg APBPeriph_CAN1
  *     @arg APBPeriph_RTZIP
  *     @arg APBPeriph_PKE
  *     @arg APBPeriph_PPE
  *     @arg APBPeriph_CAN0
  *     @arg APBPeriph_TIMER1
  *     @arg APBPeriph_TIMER1_PWM
  *     @arg APBPeriph_AAC_XTAL
  *     @arg APBPeriph_PDCK
  *     @arg APBPeriph_SWR_SS
  *     @arg APBPeriph_TIMER
  *     @arg APBPeriph_USB
  *     @arg APBPeriph_SD_HOST
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_UART5
  *     @arg APBPeriph_UART4
  *     @arg APBPeriph_UART3
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_FLASH2
  *     @arg APBPeriph_FLASH1
  *     @arg APBPeriph_FLASH
  *     @arg APBPeriph_BTBUS
  *     @arg APBPeriph_DATA_MEM
  *     @arg APBPeriph_EFUSE
  *     @arg APBPeriph_DSP_WDT
  *     @arg APBPeriph_ASRC
  *     @arg APBPeriph_DSP_MEM
  *     @arg APBPeriph_DSP_H2D_D2H
  *     @arg APBPeriph_DSP_CORE
  *     @arg APBPeriph_SPI0_SLAVE
  *     @arg APBPeriph_PSRAM
  *     @arg APBPeriph_I2C2
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_SPI2
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_SM3
  *     @arg APBPeriph_SHA256
  *     @arg APBPeriph_DISP
  *     @arg APBPeriph_ISO7816
  *     @arg APBPeriph_RNG
  *     @arg APBPeriph_AES
  *     @arg APBPeriph_GPIOB
  *     @arg APBPeriph_GPIOA
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S2
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_CODEC
  *     @arg APBPeriph_CKE_MODEM
  *     @arg APBPeriph_VENDOR_REG
  *     @arg APBPeriph_CKE_BTV
  *     @arg APBPeriph_BUS_RAM_SLP
  *     @arg APBPeriph_CKE_CTRLAP
  *     @arg APBPeriph_CKE_PLFM
  *     @arg APBPeriph_GPIO1_DEB
  *     @arg APBPeriph_GPIO0_DEB
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_I2S2_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  *     @arg APBPeriph_SD_HOST_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_FLASH2_CLOCK
  *     @arg APBPeriph_FLASH1_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_TIMER_CLOCK
  *     @arg APBPeriph_FLASH_CLOCK
  *     @arg APBPeriph_VENDOR_REG_CLOCK
  *     @arg APBPeriph_CKE_BTV_CLOCK
  *     @arg APBPeriph_BUS_RAM_SLP_CLOCK
  *     @arg APBPeriph_CKE_CTRLAP_CLOCK
  *     @arg APBPeriph_CKE_PLFM_CLOCK
  *     @arg APBPeriph_GPIO1_DEB_CLOCK
  *     @arg APBPeriph_GPIO0_DEB_CLOCK
  *     @arg APBPeriph_RTZIP_CLOCK
  *     @arg APBPeriph_PKE_CLOCK
  *     @arg APBPeriph_PPE_CLOCK
  *     @arg APBPeriph_CAN0_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_TIMER1_CLOCK
  *     @arg APBPeriph_CKE_SM3_CLOCK
  *     @arg APBPeriph_CKE_SHA256_CLOCK
  *     @arg APBPeriph_CKE_AAC_XTAL_CLOCK
  *     @arg APBPeriph_CKE_PDCK_CLOCK
  *     @arg APBPeriph_RNG_CLOCK
  *     @arg APBPeriph_SWR_SS_CLOCK
  *     @arg APBPeriph_CAL_32K_CLOCK
  *     @arg APBPeriph_CKE_MODEM_CLOCK
  *     @arg APBPeriph_SPI0_SLAVE_CLOCK
  *     @arg APBPeriph_EFUSE_CLOCK
  *     @arg APBPeriph_CKE_DSP_WDT_CLOCK
  *     @arg APBPeriph_CKE_DSP_CLOCK
  *     @arg APBPeriph_CKE_H2D_D2H
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_DSP_MEM_CLOCK
  *     @arg APBPeriph_ASRC_CLOCK
  *     @arg APBPeriph_DISP_CLOCK
  *     @arg APBPeriph_DATA_MEM1_CLOCK
  *     @arg APBPeriph_DATA_MEM0_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_ISO7816_CLOCK
  *     @arg APBPeriph_AES_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_CAN1_CLOCK
  *     @arg APBPeriph_UART5_CLOCK
  *     @arg APBPeriph_UART4_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_JPEG_CLOCK
  *     @arg APBPeriph_ZIGBEE_CLOCK
  *     @arg APBPeriph_BTBUS_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_APB_PERIPH_CLOCK(APBPeriph_Clock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /*Open clock gating first*/
    if (NewState == ENABLE)
    {
        if (IS_5M_PERI_CLK(APBPeriph_Clock))
        {
            /*Open 5M clock source*/
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_40M_SRC_DIV_EN = 1;
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_5M_SRC_EN = 1;
        }
        else if (IS_10M_PERI_CLK(APBPeriph_Clock))
        {
            /*Open 10M clock source*/
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_40M_SRC_DIV_EN = 1;
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_10M_SRC_EN = 1;
        }
        else if (IS_20M_PERI_CLK(APBPeriph_Clock))
        {
            /*Open 20M clock source*/
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_40M_SRC_DIV_EN = 1;
            SYSBLKCTRL->u_20C.BITS_20C.r_CLK_20M_SRC_EN = 1;
        }
    }

    RCC_PeriClockConfig(APBPeriph_Clock, NewState);
    RCC_PeriFunctionConfig(APBPeriph, NewState);

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
*     @arg APBPeriph_I2S2_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  *     @arg APBPeriph_SD_HOST_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_FLASH2_CLOCK
  *     @arg APBPeriph_FLASH1_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_TIMER_CLOCK
  *     @arg APBPeriph_FLASH_CLOCK
  *     @arg APBPeriph_VENDOR_REG_CLOCK
  *     @arg APBPeriph_CKE_BTV_CLOCK
  *     @arg APBPeriph_BUS_RAM_SLP_CLOCK
  *     @arg APBPeriph_CKE_CTRLAP_CLOCK
  *     @arg APBPeriph_CKE_PLFM_CLOCK
  *     @arg APBPeriph_GPIO1_DEB_CLOCK
  *     @arg APBPeriph_GPIO0_DEB_CLOCK
  *     @arg APBPeriph_RTZIP_CLOCK
  *     @arg APBPeriph_PKE_CLOCK
  *     @arg APBPeriph_PPE_CLOCK
  *     @arg APBPeriph_CAN0_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_TIMER1_CLOCK
  *     @arg APBPeriph_CKE_SM3_CLOCK
  *     @arg APBPeriph_CKE_SHA256_CLOCK
  *     @arg APBPeriph_CKE_AAC_XTAL_CLOCK
  *     @arg APBPeriph_CKE_PDCK_CLOCK
  *     @arg APBPeriph_RNG_CLOCK
  *     @arg APBPeriph_SWR_SS_CLOCK
  *     @arg APBPeriph_CAL_32K_CLOCK
  *     @arg APBPeriph_CKE_MODEM_CLOCK
  *     @arg APBPeriph_SPI0_SLAVE_CLOCK
  *     @arg APBPeriph_EFUSE_CLOCK
  *     @arg APBPeriph_CKE_DSP_WDT_CLOCK
  *     @arg APBPeriph_CKE_DSP_CLOCK
  *     @arg APBPeriph_CKE_H2D_D2H
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_DSP_MEM_CLOCK
  *     @arg APBPeriph_ASRC_CLOCK
  *     @arg APBPeriph_DISP_CLOCK
  *     @arg APBPeriph_DATA_MEM1_CLOCK
  *     @arg APBPeriph_DATA_MEM0_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_ISO7816_CLOCK
  *     @arg APBPeriph_AES_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_CAN1_CLOCK
  *     @arg APBPeriph_UART5_CLOCK
  *     @arg APBPeriph_UART4_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_JPEG_CLOCK
  *     @arg APBPeriph_ZIGBEE_CLOCK
  *     @arg APBPeriph_BTBUS_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriClockConfig(uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH_CLOCK(APBPeriph_Clock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t clkRegOff = 0;
    uint8_t is_sleep_clk_exist = 0;
    uint32_t *periph_clock_address = NULL;
    uint32_t periph_clock_value = 0;
    uint32_t reg_offset_mask = 0x07;

    /* Special register handle */
    if ((APBPeriph_Clock == APBPeriph_I2S0_CLOCK) || (APBPeriph_Clock == APBPeriph_I2S1_CLOCK) ||
        (APBPeriph_Clock == APBPeriph_CODEC_CLOCK) || (APBPeriph_Clock == APBPeriph_I2S2_CLOCK))
    {
        periph_clock_value = SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN;
        if (NewState == ENABLE)
        {
            periph_clock_value |= APBPeriph_Clock;
        }
        else
        {
            periph_clock_value &= ~(APBPeriph_Clock);
        }

        SYSBLKCTRL->u_220.SOC_AUDIO_IF_EN = periph_clock_value;

        return;
    }

    clkRegOff = ((APBPeriph_Clock & (reg_offset_mask << 29)) >> 29);
    APBPeriph_Clock &= (~(reg_offset_mask << 29));

    is_sleep_clk_exist = !(APBPeriph_Clock & APBPeriph_SLEEP_CLOCK_NOT_EXIST);
    APBPeriph_Clock &= ~APBPeriph_SLEEP_CLOCK_NOT_EXIST;

    periph_clock_address = ((uint32_t *)(&(SYSBLKCTRL->u_230.PESOC_CLK_CTRL)) + clkRegOff - 1);
    periph_clock_value = *periph_clock_address;

    if (NewState == ENABLE)
    {
        //enable peripheral clock
        periph_clock_value |= (uint32_t)((uint32_t)1 << APBPeriph_Clock);
        //enable peripheral clock in sleep mode
        if (is_sleep_clk_exist)
        {
            periph_clock_value |= (uint32_t)((uint32_t)1 << (APBPeriph_Clock + 1));
        }
    }
    else
    {
        //disable peripheral clock
        platform_delay_us(2);
        periph_clock_value &= ~((uint32_t)1 << APBPeriph_Clock);

        //disable peripheral clock in sleep mode
        if (is_sleep_clk_exist)
        {
            periph_clock_value &= ~((uint32_t)1 << (APBPeriph_Clock + 1));
        }
    }

    *periph_clock_address = periph_clock_value;

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMER1_DEB
  *     @arg APBPeriph_ZIGBEE
  *     @arg APBPeriph_GMAC
  *     @arg APBPeriph_CAN1
  *     @arg APBPeriph_RTZIP
  *     @arg APBPeriph_PKE
  *     @arg APBPeriph_PPE
  *     @arg APBPeriph_CAN0
  *     @arg APBPeriph_TIMER1
  *     @arg APBPeriph_TIMER1_PWM
  *     @arg APBPeriph_AAC_XTAL
  *     @arg APBPeriph_PDCK
  *     @arg APBPeriph_SWR_SS
  *     @arg APBPeriph_TIMER
  *     @arg APBPeriph_USB
  *     @arg APBPeriph_SD_HOST
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_UART5
  *     @arg APBPeriph_UART4
  *     @arg APBPeriph_UART3
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_FLASH2
  *     @arg APBPeriph_FLASH1
  *     @arg APBPeriph_FLASH
  *     @arg APBPeriph_BTBUS
  *     @arg APBPeriph_DATA_MEM
  *     @arg APBPeriph_EFUSE
  *     @arg APBPeriph_DSP_WDT
  *     @arg APBPeriph_ASRC
  *     @arg APBPeriph_DSP_MEM
  *     @arg APBPeriph_DSP_H2D_D2H
  *     @arg APBPeriph_DSP_CORE
  *     @arg APBPeriph_SPI0_SLAVE
  *     @arg APBPeriph_PSRAM
  *     @arg APBPeriph_I2C2
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_SPI2
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_SM3
  *     @arg APBPeriph_SHA256
  *     @arg APBPeriph_DISP
  *     @arg APBPeriph_ISO7816
  *     @arg APBPeriph_RNG
  *     @arg APBPeriph_AES
  *     @arg APBPeriph_GPIOB
  *     @arg APBPeriph_GPIOA
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S2
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_CODEC
  *     @arg APBPeriph_CKE_MODEM
  *     @arg APBPeriph_VENDOR_REG
  *     @arg APBPeriph_CKE_BTV
  *     @arg APBPeriph_BUS_RAM_SLP
  *     @arg APBPeriph_CKE_CTRLAP
  *     @arg APBPeriph_CKE_PLFM
  *     @arg APBPeriph_GPIO1_DEB
  *     @arg APBPeriph_GPIO0_DEB
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriFunctionConfig(uint32_t APBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (APBPeriph == APBPeriph_NO_FUNCTION_BIT)
    {
        return;
    }

    uint32_t apbRegOff = 0;
    uint32_t *periph_function_address = NULL;
    uint32_t periph_function_value = 0;
    uint32_t reg_offset_mask = 0x07;

    /* clear flag */
    apbRegOff = ((APBPeriph & (reg_offset_mask << 26)) >> 26);
    APBPeriph &= (~(reg_offset_mask << 26));

    periph_function_address = ((uint32_t *)(&(SYSBLKCTRL->u_210.SOC_FUNC_EN)) + apbRegOff);
    periph_function_value = *periph_function_address;
    if (NewState == ENABLE)
    {
        //enable peripheral
        periph_function_value |= (uint32_t)((uint32_t)1 << APBPeriph);
    }
    else
    {
        /*delay 2us or aux adc potential hang*/
        platform_delay_us(2);
        //disable peripheral
        periph_function_value &= ~((uint32_t)1 << APBPeriph);
    }

    *periph_function_address = periph_function_value;

    return;
}
