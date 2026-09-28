/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl_driver_entry.c
* @brief    This file provides the driver entry api.
* @details
* @author
* @date     2024-09-02
* @version  v1.0
*********************************************************************************************************
*/
void driver_entry(void)
{
    extern void GPIO_EXTIInit(void);
    GPIO_EXTIInit();
}
