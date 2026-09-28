/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl_utils.h
* @brief     This header file provide the basic driver utility definitions.
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *                         Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_UTILS_H
#define RTL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                         Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "bitops.h"
#include "cpu_setting.h"

/*============================================================================*
 *                         Types
*============================================================================*/
/** \defgroup UTILS       UTILS
  * \brief
  * \{
  */

/** \defgroup UTILS_Exported_Constants UTILS Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    ITStatus ITStatus
 * \{
 * \ingroup     UTILS_Exported_Constants
 */
typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus, ITStatus;

/** End of ITStatus
  * \}
  */

/**
 * \defgroup    FunctionalState FunctionalState
 * \{
 * \ingroup     UTILS_Exported_Constants
 */
typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

/** End of FunctionalState
  * \}
  */

/** End of UTILS_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup UTILS_Exported_Functions UTILS Exported Functions
  * \brief
  * \{
  */

/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
//#define USE_FULL_ASSERT

/**
  * \brief  The assert_param macro is used for function's parameters check.
  * \param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * \retval None
  */
#ifdef USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
void io_assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

/** End of UTILS_Exported_Functions
  * \}
  */

/** End of UTILS
  * \}
  */
#ifdef __cplusplus
}
#endif

#endif /* RTL_UTILS_H */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
