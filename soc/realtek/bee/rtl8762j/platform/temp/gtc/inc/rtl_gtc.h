/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_gtc.h
* \brief    The header file of the peripheral GTC driver.
* \details  This file provides all GTC firmware functions.
* \author   grace_yan
* \date     2024-11-22
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_GTC_H
#define RTL_GTC_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "utils/rtl_utils.h"

/** \defgroup GTC         GTC
  * \brief
  * \{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup GTC_Exported_Functions GTC Exported Functions
  * \brief
  * \{
  */

/**
 * \brief     Get the counter value of GTC.
 *
 * \param[in] None.
 *
 * \return    The counter value.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gtc_demo(void)
 * {
 *     uint32_t counter = GTC_GetCounter();
 * }
 * \endcode
 */
uint32_t GTC_GetCounter(void);

/** End of GTC_Exported_Functions
  * \}
  */

/** End of GTC
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL_GTC_H */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
