/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* \file     rtl_rcc.c
* \brief    This file provides all the IO clock firmware functions.
* \details
* \author   Bert
* \date     2024-05-08
* \version  v0.1
*********************************************************************************************************
*/

/*============================================================================*
 *                           Header Files
 *============================================================================*/
#include "rtl_rcc.h"

/*============================================================================*
 *                           Private Defines
 *============================================================================*/
#define RCC_CLOCK_ADDR(n)           (*((volatile uint32_t *)(SYSON_BASE + PCC_REG_PERION_CLK_VPON_REG0X) + n))

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void RCC_ClockCmd(uint32_t PeriphClock, FunctionalState NewState)
{
    /* Check the parameters. */
    assert_param(IS_PERIPH_CLOCK(PeriphClock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    uint32_t RegOffset = (PeriphClock >> 24);
    uint32_t RegClock  = (PeriphClock << 8 >> 8);

    if (NewState == ENABLE)
    {
        RCC_CLOCK_ADDR(RegOffset) |= BIT(RegClock);

        if (PeriphClock == TIMER1_CLOCK)
        {
            /* TIMER1_Clock_PWM6 TIMER1_Clock_PWM7 TIMER1_Clock_PWM8 */
            RCC_CLOCK_ADDR(RegOffset + 1) |= (BIT(22) | BIT(23) | BIT(24));
        }
        else if (PeriphClock == TIMER2_CLOCK)
        {
            /* TIMER2_Clock_PWM */
            RCC_CLOCK_ADDR(RegOffset) |= BIT(17);
            /* TIMER2_Clock_GPIO_DBNC_2P4G for GPIO LATCH DEBOUNCE */
            RCC_CLOCK_ADDR(RegOffset) |= BIT(8);

        }
    }
    else
    {
        RCC_CLOCK_ADDR(RegOffset) &= ~BIT(RegClock);

        if (PeriphClock == TIMER1_CLOCK)
        {
            /* Clock_TIMER1_PWM6 Clock_TIMER1_PWM7 Clock_TIMER1_PWM8 */
            RCC_CLOCK_ADDR(RegOffset + 1) &= ~(BIT(22) | BIT(23) | BIT(24));
        }
        else if (PeriphClock == TIMER2_CLOCK)
        {
            /* Clock_TIMER2_PWM */
            RCC_CLOCK_ADDR(RegOffset) &= ~BIT(17);
            /* TIMER2_Clock_GPIO_DBNC_2P4G for GPIO LATCH DEBOUNCE */
            RCC_CLOCK_ADDR(RegOffset) &= ~ BIT(8);
        }
    }

    return;
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
