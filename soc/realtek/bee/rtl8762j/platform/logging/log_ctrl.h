/**
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _LOG_CFG_H_
#define _LOG_CFG_H_

#include <stdint.h>
#include <stdbool.h>
#include "soc_log.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \defgroup    TRACE       Trace
 *
 * \brief       Defines debug trace macros for each module.
 *
 */

/**@}*/

#define LOG_OUTPUT_UART_MASK        BIT0
#define LOG_OUTPUT_FLASH_MASK       BIT1
#define LOG_OUTPUT_VENDOR_MASK      BIT2


/**
 * trace.h
 *
 * \brief    Initialize module trace mask.
 *
 * \param[in]   mask    Module trace mask array. Set NULL to load default mask array.
 *
 * \return      None.
 *
 * \ingroup  TRACE
 */
/**
 * trace.h
 *
 * \brief    Initialize module trace mask.
 *
 * \param[in]   mask    Module trace mask array. Set NULL to load default mask array.
 *
 * \return      None.
 *
 * \ingroup  TRACE
 */
void log_module_trace_init(uint64_t mask[LEVEL_NUM]);

/**
 * trace.h
 *
 * \brief    Enable/Disable the module ID's trace.
 *
 * \param[in]   module_id   The specific module ID defined in \ref MODULE_ID.
 *
 * \param[in]   trace_level The trace level of the module ID defined in \ref TRACE_LEVEL.
 *
 * \param[in]   set         Enable or disable the module ID's trace.
 * \arg \c true     Enable the module ID's trace.
 * \arg \c false    Disable the module ID's trace.
 *
 * \return           The status of setting module ID's trace.
 * \retval true      Module ID's trace was set successfully.
 * \retval false     Module ID's trace was failed to set.
 *
 * \ingroup  TRACE
 */
bool log_module_trace_set(T_MODULE_ID module_id, uint8_t trace_level, bool set);

/**
 * trace.h
 *
 * \brief    Enable/Disable module bitmap's trace.
 *
 * \param[in]   module_bitmap   The module bitmap defined in \ref MODULE_BITMAP.
 *
 * \param[in]   trace_level     The trace level of the module bitmap defined in \ref TRACE_LEVEL.
 *
 * \param[in]   set             Enable or disable the module bitmap's trace.
 * \arg \c true     Enable the module bitmap's trace.
 * \arg \c false    Disable the module bitmap's trace.
 *
 * \return           The status of setting module bitmap's trace.
 * \retval true      Module bitmap's trace was set successfully.
 * \retval false     Module bitmap's trace was failed to set.
 *
 * \ingroup  TRACE
 */
bool log_module_bitmap_trace_set(uint64_t module_bitmap, uint8_t trace_level, bool set);

///**
// * trace.h
// *
// * \brief    Get system timestamp.
// *
// * \param       None.
// *
// * \return      System timestamp value in milliseconds.
// *
// * \ingroup  TRACE
// */
//extern uint32_t sys_timestamp_get(void);
//extern uint32_t log_timestamp_get(void);

/**
 * trace.h
 *
 * \brief    Register log destination callback function.
 *
 * \param[in]   dest    Indicates where the callback is registered in.
 * \arg \c LOG_OUTPUT_FLASH_MASK    Register callback for log to flash.
 * \arg \c LOG_OUTPUT_VENDOR_MASK   Register callback for vendor specific use.
 *
 * \param[in]   func    The callback function to register.
 *
 * \return      None.
 *
 * \ingroup  TRACE
 */
void register_log_dest_cb(uint8_t dest, void *func);


#ifdef __cplusplus
}
#endif

#endif /* _TRACE_H_ */
