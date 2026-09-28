/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl_rcc.c
* @brief    This file provides all the IO clock firmware functions..
* @details
* @author   Echo_gao
* @date     2023-07-11
* @version  v2.0
*********************************************************************************************************
*/
#include <stdint.h>
#include "rtl876x.h"
#include "rtl_rcc.h"

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *         This parameter can refer APB Peripheral.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *         This parameter can refer to APB Peripheral Clock (must be the same with APBPeriph).
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be: ENABLE or DISABLE.
  * @return None
  */
void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters. */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t tmp_1 = APBPeriph & 0x800;
    uint32_t tmp_2 = APBPeriph & 0x100;
    uint32_t tmp_3 = APBPeriph & 0xA00;
    uint32_t apbRegOff = (APBPeriph & (0XFF));
    uint32_t clk = APBPeriph_Clock;
    uint32_t clk_func = APBPeriph_Clock;

    if (tmp_1 == 0x800)
    {
        clk_func += clk << 1;
    }
    else
    {
        clk_func += clk >> 1;
    }

    if (NewState == ENABLE)
    {
        /* Enable peripheral and peripheral clock */
        if (tmp_2 == 0x0100)
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK1->u_900.REG_SOC_AUDIO_IF_EN)) + apbRegOff) |= clk_func;
        }
        else if (tmp_3 == 0x0A00)  //for GPU
        {
            PERIBLKCTRL_PERI_CLK->u_1E4.BITS_1E4.gpu_ck_en = 0x1;
            //delay 128 clock cycle
            for (uint16_t i = 0; i < 70 ; i++);
            PERIBLKCTRL_PERI_CLK->u_1E4.BITS_1E4.gpu_func_en = 0x1;
            //delay 128 clock cycle
            for (uint16_t i = 0; i < 70 ; i++);
        }
        else
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK->u_100.REG_PERI_SPIC0_CTL)) + apbRegOff) |= clk_func;
        }
    }
    else
    {
        /* Disable peripheral and peripheral clock */
        if (tmp_2 == 0x0100)
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK1->u_900.REG_SOC_AUDIO_IF_EN)) + apbRegOff) &= (~clk_func);
        }
        else
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK->u_100.REG_PERI_SPIC0_CTL)) + apbRegOff) &= (~clk_func);
        }
    }

    return;
}
/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *         This parameter can refer APB Peripheral.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *         This parameter can refer to APB Peripheral Clock (must be the same with APBPeriph).
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be: ENABLE or DISABLE.
  * @return None
  */
void RCC_PeriFunctionConfig(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* Check the parameters. */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t tmp_1 = APBPeriph & 0x800;
    uint32_t tmp_2 = APBPeriph & 0x100;
    uint32_t apbRegOff = (APBPeriph & (0XFF));
    uint32_t clk = APBPeriph_Clock;
    uint32_t clk_func = APBPeriph_Clock;

    if (tmp_1 == 0x800)
    {
        clk_func = clk << 1;
    }
    else
    {
        clk_func = clk >> 1;
    }

    if (NewState == ENABLE)
    {
        /* Enable peripheral and peripheral clock */
        if (tmp_2 == 0x0100)
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK1->u_900.REG_SOC_AUDIO_IF_EN)) + apbRegOff) |= clk_func;
        }
        else
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK->u_100.REG_PERI_SPIC0_CTL)) + apbRegOff) |= clk_func;
        }
    }
    else
    {
        /* Disable peripheral and peripheral clock */
        if (tmp_2 == 0x0100)
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK1->u_900.REG_SOC_AUDIO_IF_EN)) + apbRegOff) &= (~clk_func);
        }
        else
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK->u_100.REG_PERI_SPIC0_CTL)) + apbRegOff) &= (~clk_func);
        }
    }

    return;
}

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: Specifies the APB peripheral to gates its clock.
  *         This parameter can refer APB Peripheral.
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *         This parameter can refer to APB Peripheral Clock (must be the same with APBPeriph).
  * @param  NewState: new state of the specified peripheral clock.
  *         This parameter can be: ENABLE or DISABLE.
  * @return None
  */
void RCC_PeriClockConfig(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters. */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t tmp_1 = APBPeriph & 0x800;
    uint32_t tmp_2 = APBPeriph & 0x100;
    uint32_t apbRegOff = (APBPeriph & (0XFF));
    uint32_t clk = APBPeriph_Clock;

    if (NewState == ENABLE)
    {
        /* Enable peripheral and peripheral clock */
        if (tmp_2 == 0x0100)
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK1->u_900.REG_SOC_AUDIO_IF_EN)) + apbRegOff) |= clk;
        }
        else
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK->u_100.REG_PERI_SPIC0_CTL)) + apbRegOff) |= clk;
        }
    }
    else
    {
        /* Disable peripheral and peripheral clock */
        if (tmp_2 == 0x0100)
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK1->u_900.REG_SOC_AUDIO_IF_EN)) + apbRegOff) &= (~clk);
        }
        else
        {
            *((uint32_t *)(&(PERIBLKCTRL_PERI_CLK->u_100.REG_PERI_SPIC0_CTL)) + apbRegOff) &= (~clk);
        }
    }

    return;
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
