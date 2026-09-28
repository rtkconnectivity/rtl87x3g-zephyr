/**
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _LOG_TRACE_H_
#define _LOG_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IMG_TRACE_ADR             (0x18000000)
#define MAX_IMG_TRACE_SIZE        (0x00FFFFFF)
#define ROM_TRACE_START           IMG_TRACE_ADR
#define ROM_TRACE_SIZE            (0x10000)
#define PLATFORM_TRACE_START      (ROM_TRACE_START + ROM_TRACE_SIZE)
#define PLATFORM_TRACE_SIZE       (0x10000)
#define PATCH_TRACE_START         (PLATFORM_TRACE_START + PLATFORM_TRACE_SIZE)
#define PATCH_TRACE_SIZE          (0x10000)
#define BTHOST_TRACE_START        (PATCH_TRACE_START + PATCH_TRACE_SIZE)
#define BTHOST_TRACE_SIZE         (0x40000)
#define BTHOST_PATCH_TRACE_START  (BTHOST_TRACE_START + BTHOST_TRACE_SIZE)
#define BTHOST_PATCH_TRACE_SIZE   (0x10000)
#define APP_TRACE_START           (BTHOST_PATCH_TRACE_START + BTHOST_PATCH_TRACE_SIZE)
#define APP_TRACE_SIZE            (0x80000)

#ifdef __cplusplus
}
#endif

#endif /* _LOG_TRACE_H_ */
