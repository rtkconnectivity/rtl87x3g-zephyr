/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl_rtc_int.c
* @brief    This file provides all the RTC firmware internal functions.
* @details
* @author
* @date     2024-09-05
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl_rtc.h"

#define RTC_AUTO_RELOAD_BIT_OFFSET  (24)

void RTC_CompAutoReloadCmd(RTCComIndex_TypeDef index, FunctionalState NewState)
{
    RTC_CR0_TypeDef rtc_0x00 = {.d32 = RTC->RTC_CR0};

    if (NewState == ENABLE)
    {
        rtc_0x00.d32 |= BIT(index + RTC_AUTO_RELOAD_BIT_OFFSET);
    }
    else
    {
        rtc_0x00.d32 &= (~BIT(index + RTC_AUTO_RELOAD_BIT_OFFSET));
    }

    aon_indirect_write_reg_safe((((uint32_t)RTC_REG_OFFSET(RTC_CR0)) | BIT31), rtc_0x00.d32);
}

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

