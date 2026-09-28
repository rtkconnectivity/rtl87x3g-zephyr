
/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_rcc.c
* @brief    This file provides all the IO clock firmware functions..
* @details
* @author   tifnan_ge
* @date     2023-02-10
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl_rcc.h"
#include "pcc_reg.h"
#include "utils.h"
#include "aon_reg.h"
#include "staging/aon/inc/indirect_access.h"


#define IS_5M_PERI_CLK(CLOCK)     ((CLOCK) == APBPeriph_KEYSCAN_CLOCK)
#define IS_10M_PERI_CLK(CLOCK)    ((CLOCK) == APBPeriph_ADC_CLOCK)
#define IS_20M_PERI_CLK(CLOCK)    (((CLOCK) == APBPeriph_QDEC_CLOCK) || (CLOCK == APBPeriph_SPI3W_CLOCK))
#define IS_TIMER_CLK(CLOCK)    (((CLOCK) == APBPeriph_TIMERA_CLOCK) || (CLOCK == APBPeriph_TIMERB_CLOCK))

#define RCC_REG_OFFSET_MASK           (0x07)

/*
    It would be better to place the following flow at the HAL layer.
*/
__weak void RCC_UpdateLPMRetentionTable(uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMERA
  *     @arg APBPeriph_TIMERB
  *     @arg APBPeriph_DMA
  *     @arg APBPeriph_LCD
  *     @arg APBPeriph_SPI3W
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C2
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI2
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_UART3
  *     @arg APBPeriph_GPIOA
  *     @arg APBPeriph_GPIOB
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_CODEC
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_TIMERA_CLOCK
  *     @arg APBPeriph_TIMERB_CLOCK
  *     @arg APBPeriph_DMA_CLOCK
  *     @arg APBPeriph_LCD_CLOCK
  *     @arg APBPeriph_SPI3W_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
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

    uint32_t apbRegOff = ((APBPeriph & ((uint32_t)RCC_REG_OFFSET_MASK << 26)) >> 26);
    uint32_t clkRegOff = ((APBPeriph_Clock & ((uint32_t)RCC_REG_OFFSET_MASK << 29)) >> 29);

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
        else if (IS_TIMER_CLK(APBPeriph_Clock))
        {
            /* Enable TIM1~2 TIM9~TIM12 fixed 40M  tim15*/
            SYSBLKCTRL->u_360.BITS_360.BIT_TIMER_CLK_f40M_EN = 1;
            SYSBLKCTRL->u_348.BITS_348.r_timer_clk_src_sel_0 = 1;
            SYSBLKCTRL->u_348.BITS_348.r_timer_mux_1_clk_cg_en = 1;
            SYSBLKCTRL->u_348.BITS_348.r_timer_cg_en = 1;
        }
    }
    else
    {
        /*If disable 5M clock, delay 2us*/
        if (IS_5M_PERI_CLK(APBPeriph_Clock))
        {
            platform_delay_us(2);
        }
        /*If disable 10M clock, delay 1us*/
        if (IS_10M_PERI_CLK(APBPeriph_Clock))
        {
            platform_delay_us(1);
        }
    }
    /* Special register handle */
    if (NewState == ENABLE)
    {
        if ((APBPeriph_Clock == APBPeriph_UART1_HCI_CLOCK) ||
            ((APBPeriph_Clock == APBPeriph_CKE_MODEM_CLOCK)))
        {
            SYSBLKCTRL->u_234.REG_PERION_REG_PESOC_PERI_CLK_CTRL0 |=  APBPeriph_Clock;
            return;
        }

    }
    else
    {
        if ((APBPeriph_Clock == APBPeriph_UART1_HCI_CLOCK) ||
            ((APBPeriph_Clock == APBPeriph_CKE_MODEM_CLOCK)))
        {
            SYSBLKCTRL->u_234.REG_PERION_REG_PESOC_PERI_CLK_CTRL0 &= ~(APBPeriph_Clock);
            return;
        }

    }

    RCC_UpdateLPMRetentionTable(APBPeriph_Clock, NewState);

    /* clear flag */
    APBPeriph &= (~(RCC_REG_OFFSET_MASK << 26));
    APBPeriph_Clock &= (~((uint32_t)RCC_REG_OFFSET_MASK << 29));
    uint8_t is_sleep_clk = (APBPeriph_Clock & BIT10) ? 0 : 1 ;
    APBPeriph_Clock &= ~BIT10;
    if (NewState == ENABLE)
    {
        //enable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.REG_PERION_REG_SOC_FUNC_EN)) + apbRegOff) |= (uint32_t)((
                uint32_t)1 << APBPeriph);
        //enable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((
                    uint32_t)1 << APBPeriph_Clock);
        //enable peripheral clock in sleep mode
        if (is_sleep_clk)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((uint32_t)1 << (APBPeriph_Clock + 1));}
    }
    else
    {
        //disable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.REG_PERION_REG_SOC_FUNC_EN)) + apbRegOff) &= ~((
                uint32_t)1 << APBPeriph);
        //disable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((
                    uint32_t)1 << APBPeriph_Clock);
        //disable peripheral clock in sleep mode
        if (is_sleep_clk)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((uint32_t)1 << (APBPeriph_Clock + 1));}
    }

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  ClockGate: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg CLOCK_GATE_5M
  *     @arg CLOCK_GATE_20M
  *     @arg CLOCK_GATE_10M
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ClockGateCmd(uint32_t ClockGate, FunctionalState NewState)
{
    assert_param(IS_CLOCK_GATE(ClockGate));

    if (NewState == ENABLE)
    {
        /* Open 40M clock source first */
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2 |= BIT20;
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2 |= BIT21;
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2 |= BIT22;
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2 |= BIT26;
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2 |= ClockGate;
    }
    else
    {
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2 &= ~ClockGate;
    }

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_TIMERA_CLOCK
  *     @arg APBPeriph_TIMERB_CLOCK
  *     @arg APBPeriph_DMA_CLOCK
  *     @arg APBPeriph_SPI3W_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C2_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK
  *     @arg APBPeriph_I2C0_CLOCK
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI2_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK
  *     @arg APBPeriph_UART0_CLOCK
  *     @arg APBPeriph_UART1_CLOCK
  *     @arg APBPeriph_UART2_CLOCK
  *     @arg APBPeriph_UART3_CLOCK
  *     @arg APBPeriph_GPIOA_CLOCK
  *     @arg APBPeriph_GPIOB_CLOCK
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_I2S0_CLOCK
  *     @arg APBPeriph_I2S1_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriClockConfig(uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH_CLOCK(APBPeriph_Clock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* Special register handle */
    if (NewState == ENABLE)
    {
        if ((APBPeriph_Clock == APBPeriph_UART1_HCI_CLOCK) ||
            ((APBPeriph_Clock == APBPeriph_CKE_MODEM_CLOCK)))
        {
            SYSBLKCTRL->u_234.REG_PERION_REG_PESOC_PERI_CLK_CTRL0 |=  APBPeriph_Clock;
            return;
        }

    }
    else
    {
        if ((APBPeriph_Clock == APBPeriph_UART1_HCI_CLOCK) ||
            ((APBPeriph_Clock == APBPeriph_CKE_MODEM_CLOCK)))
        {
            SYSBLKCTRL->u_234.REG_PERION_REG_PESOC_PERI_CLK_CTRL0 &= ~(APBPeriph_Clock);
            return;
        }

    }

    uint32_t clkRegOff = ((APBPeriph_Clock & (0x03 << 29)) >> 29);
    APBPeriph_Clock &= (~(0x03 << 29));
    uint8_t is_double_clk = (APBPeriph_Clock & BIT10) ? 0 : 1 ;
    APBPeriph_Clock &= ~BIT10;

    if (NewState == ENABLE)
    {
        //enable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((
                    uint32_t)1 << APBPeriph_Clock);
        //enable peripheral clock in sleep mode
        if (is_double_clk)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) |= (uint32_t)((uint32_t)1 << (APBPeriph_Clock + 1));}
    }
    else
    {
        //disable peripheral clock
        *((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((
                    uint32_t)1 << APBPeriph_Clock);
        //disable peripheral clock in sleep mode
        if (is_double_clk)
        {*((uint32_t *)(&(SYSBLKCTRL->u_230.REG_PERION_REG_PESOC_CLK_CTRL)) + clkRegOff - 1) &= ~((uint32_t)1 << (APBPeriph_Clock + 1));}
    }

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:
  *     @arg APBPeriph_TIMERA B
  *     @arg APBPeriph_DMA
  *     @arg APBPeriph_LCD
  *     @arg APBPeriph_SPI3W
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C1
  *     @arg APBPeriph_I2C0
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0
  *     @arg APBPeriph_UART0
  *     @arg APBPeriph_UART1
  *     @arg APBPeriph_UART2
  *     @arg APBPeriph_GPIO
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_I2S0
  *     @arg APBPeriph_I2S1
  *     @arg APBPeriph_CODEC
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriFunctionConfig(uint32_t APBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t apbRegOff = ((APBPeriph & (0x03 << 26)) >> 26);

    if (APBPeriph == APBPeriph_UART1_HCI)
    {
        return;
    }
    /* clear flag */
    APBPeriph &= (~(0x03 << 26));

    if (NewState == ENABLE)
    {
        //enable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.REG_PERION_REG_SOC_FUNC_EN)) + apbRegOff) |= (uint32_t)((
                uint32_t)1 << APBPeriph);
    }
    else
    {
        //disable peripheral
        *((uint32_t *)(&(SYSBLKCTRL->u_210.REG_PERION_REG_SOC_FUNC_EN)) + apbRegOff) &= ~((
                uint32_t)1 << APBPeriph);
    }

    return;
}

/**
  * @brief  SPDIF clock source config.
  * @param  ClockSource: specifies the SPDIF clock sorce.
  *      this parameter can be one of the following values:
  *     @arg SPDIF_CLOCK_SOURCE_40M
  *     @arg SPDIF_CLOCK_SOURCE_PLL
  *     @arg SPDIF_CLOCK_SOURCE_PLL2
  *     @arg SPDIF_CLOCK_SOURCE_PLL3
  *     @arg SPDIF_CLOCK_SOURCE_MCLK
  * @param  first_div: specifies the SPDIF clock souce first div, actual effective division value = first_div + 1.
  * @param  sec_div: specifies the SPDIF clock souce second div.
  *      this parameter can be one of the following values:
  *     @arg SPDIF_CLOCK_SEC_DIV_DISABLE
  *     @arg SPDIF_CLOCK_SEC_DIV_2
  * @retval None
  */
void RCC_SPDIFClkSourceConfig(uint16_t ClockSource, uint8_t first_div, uint8_t sec_div)
{
    /* Check the parameters */
    assert_param(IS_SPDIF_SOR(ClockSource));
    assert_param(IS_SPDIF_SEC_DIV(sec_div));

    /* spdif_slave_mode = 0x40508004[3] */
    uint8_t spdif_slave_mode = 0;
    uint8_t r_clk_en_spdif = 0;

    /* r_PLL_DIV4_SETTING = AON_reg 0x6F6[15:8] */
    AON_FAST_REG3X_BTPLL_SYS_TYPE reg3x_btpll_sys = {.d16 = btaon_fast_read_safe(AON_FAST_REG3X_BTPLL_SYS)};
    /* r_SPDIF_PLL_1_2_SEL = AON_fast_reg 0x6FE[4] */
    AON_FAST_REG1X_CORE_SYS_TYPE reg1x_core_sys = {.d16 = btaon_fast_read_safe(AON_FAST_REG1X_CORE_SYS)};
    /* spdif_dsp_clk_sel = AON_fast_reg 0x700[3:1] */
    AON_FAST_REG2X_CORE_SYS_TYPE reg2x_core_sys = {.d16 = btaon_fast_read_safe(AON_FAST_REG2X_CORE_SYS)};
    /* r_CLK_EN_SPDIF = AON_reg 0x702[9] */
    AON_FAST_REG3X_CORE_SYS_TYPE reg3x_core_sys = {.d16 = btaon_fast_read_safe(AON_FAST_REG3X_CORE_SYS)};
    /* r_CLK_40M_VCORE_EN = r_aon_clk_cg_en16 = AON_fast_reg 0x716[10] */
    AON_FAST_REG8X_CORE_SYS_TYPE reg8x_core_sys = {.d16 = btaon_fast_read_safe(AON_FAST_REG8X_CORE_SYS)};

    if (ClockSource == SPDIF_CLOCK_SOURCE_40M)
    {
        r_clk_en_spdif = 0;
        reg8x_core_sys.r_aon_clk_cg_en16 = 1;
        reg2x_core_sys.spdif_dsp_clk_sel |= BIT(2);
    }
    else
    {
        r_clk_en_spdif = 1;
        reg8x_core_sys.r_aon_clk_cg_en16 = 0;
        reg2x_core_sys.spdif_dsp_clk_sel &= ~BIT(2);

        if (ClockSource == SPDIF_CLOCK_SOURCE_MCLK)
        {
            spdif_slave_mode = 1;
        }
        else
        {
            spdif_slave_mode = 0;

            if (ClockSource == SPDIF_CLOCK_SOURCE_PLL3)
            {
                /* r_SPDIF_PLL_3_SEL = 0x40000228[0] */
                SYSBLKCTRL->REG_PERION_REG_SOC_AUDIO_CLK_CTRL_B |= BIT(0);
            }
            else
            {
                /* r_SPDIF_PLL_3_SEL = 0x40000228[0] */
                SYSBLKCTRL->REG_PERION_REG_SOC_AUDIO_CLK_CTRL_B &= ~BIT(0);

                if (ClockSource == SPDIF_CLOCK_SOURCE_PLL2)
                {
                    reg1x_core_sys.r_SPDIF_PLL_1_2_SEL = 1;
                }
                else if (ClockSource == SPDIF_CLOCK_SOURCE_PLL)
                {
                    reg1x_core_sys.r_SPDIF_PLL_1_2_SEL = 0;
                }
            }
        }
    }

    if (first_div != 0)
    {
        reg2x_core_sys.spdif_dsp_clk_sel |= BIT(0);
        reg3x_btpll_sys.r_PLL_DIV4_SETTING = first_div;
        if (sec_div == SPDIF_CLOCK_SEC_DIV_2)
        {
            reg2x_core_sys.spdif_dsp_clk_sel |= BIT(1);
        }
        else
        {
            reg2x_core_sys.spdif_dsp_clk_sel &= ~BIT(1);
        }
    }
    else
    {
        reg2x_core_sys.spdif_dsp_clk_sel &= ~BIT(0);
        reg2x_core_sys.spdif_dsp_clk_sel &= ~BIT(1);
    }

    btaon_fast_write_safe(AON_FAST_REG1X_CORE_SYS, reg1x_core_sys.d16);
    btaon_fast_write_safe(AON_FAST_REG8X_CORE_SYS, reg8x_core_sys.d16);
    btaon_fast_write_safe(AON_FAST_REG3X_BTPLL_SYS, reg3x_btpll_sys.d16);
    btaon_fast_write_safe(AON_FAST_REG2X_CORE_SYS, reg2x_core_sys.d16);

    /* open spdif clock use cpu bus */
    if (r_clk_en_spdif)
    {
        reg3x_core_sys.r_CLK_EN_SPDIF = 1;
    }
    else
    {
        reg3x_core_sys.r_CLK_EN_SPDIF = 0;
    }
    reg3x_core_sys.r_FEN_SPDIF = 1;
    /* this register must last write to prevent glitch*/
    btaon_fast_write_safe(AON_FAST_REG3X_CORE_SYS, reg3x_core_sys.d16);

    /* only can be access when open spdif clock */
    if (spdif_slave_mode)
    {
        SPDIF_REG_04 |= BIT(3);
    }
    else
    {
        SPDIF_REG_04 &= ~BIT(3);
    }

    return;
}

