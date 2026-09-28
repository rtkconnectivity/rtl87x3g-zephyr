#ifndef _OS_POWER_MANAGER_H_
#define _OS_POWER_MANAGER_H_

#include "platform_utils.h"
#include "os_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UPDATE_TICK_COUNT()\
    if (portNVIC_INT_CTRL_REG & SCB_ICSR_PENDSTSET_Msk)\
    {\
        os_systick_handler();\
        portNVIC_INT_CTRL_REG = SCB_ICSR_PENDSTCLR_Msk;\
    }

#define US_TO_TICK_COUNT(us, residual)\
    cal_quotient_remainder(1000000 * (uint64_t)(os_sys_tick_clk_get() / os_sys_tick_rate_get()), configSYSTICK_CLOCK_HZ, us, &residual)

#define US_TO_SYSTICK(us, residual)\
    cal_quotient_remainder(1000000, os_sys_tick_clk_get(), us, &residual)

#define TICK_COUNT_TO_US(tick_count)\
    cal_quotient_remainder(os_sys_tick_clk_get(), 1000000 * (uint64_t)(os_sys_tick_clk_get() / os_sys_tick_rate_get()), tick_count, NULL)

#define SYSTICK_TO_US(systick)\
    cal_quotient_remainder(os_sys_tick_clk_get(), 1000000, systick, NULL)

#define SYSTICK_RELOAD_VALUE    (os_sys_tick_clk_get() /  os_sys_tick_rate_get() - 1)

typedef struct _PlatformPMBufferBackup
{
    bool has_dynamic_buffer;
    uint8_t *buffer_stored_addr;
    T_OS_QUEUE buffer_queue;
} PlatformPMBufferBackup;

typedef enum
{
    PLATFORM_PM_EXCLUDED_TIMER,
    PLATFORM_PM_EXCLUDED_TASK,
    PLATFORM_PM_EXCLUDED_TYPE_MAX,
} PlatformExcludedHandleType;

typedef struct _PlatformPMExcludedHandleQueueElem
{
    struct PlatformPMExcludedHandleQueueElem *pNext;
    void **handle;
} PlatformPMExcludedHandleQueueElem;

typedef union
{
    uint8_t value[1];
    struct
    {
        uint8_t os_pm_statistic:        1;
        uint8_t rsvd:                   7;
    };
} OSPMFeatureConfig;

typedef struct
{
    uint32_t last_sleep_clk;
    uint32_t last_sleep_systick;
    uint32_t tickcount_restore_remain_us;
} OSPMSystem;

typedef struct _PlatformPMBufferQueueElem
{
    struct _PlatformPMBufferQueueElem *pNext;
    uint8_t *pBufferStart;
    uint8_t *pBackup;
    uint32_t length : 31;
    uint32_t shouldFree : 1;
} PlatformPMBufferQueueElem;

uint32_t os_sys_tick_rate_get(void);
uint32_t os_sys_tick_clk_get(void);
uint32_t get_sys_tick_clk_type(void);

bool os_register_pm_excluded_handle(void **handle, PlatformExcludedHandleType type);
bool os_unregister_pm_excluded_handle(void **handle, PlatformExcludedHandleType type);

void os_pm_stop_all_non_excluded_timer(void);

bool os_register_pm_buffer(uint8_t *pBufferStart, uint32_t length, bool shouldFree);
void os_unregister_pm_buffer(uint8_t *pBufferStart);
uint16_t os_pm_get_buffer_store_count(void);
uint32_t os_pm_get_valid_systick_current_value(bool need_update);

void os_pm_init(void);
void os_pm_restore_os_tick_count(void);

extern bool (*patch_osif_os_pm_bottom_half)(void *);
extern bool (*patch_osif_os_pm_return_idle_task)(void);
extern bool (*patch_osif_os_pm_find_nearest_timeout_tick)(uint32_t *ret);
extern bool (*patch_osif_os_pm_restore_os_tick_count)(void);
extern bool (*patch_osif_os_register_pm_excluded_handle)(void **handle,
                                                         PlatformExcludedHandleType type, bool *ret);
extern bool (*patch_osif_os_unregister_pm_excluded_handle)(void **handle,
                                                           PlatformExcludedHandleType type, bool *ret);
extern bool (*patch_osif_os_pm_stop_all_non_excluded_timer)(void);
extern bool (*patch_osif_os_pm_get_valid_systick_current_value)(bool need_update, uint32_t *ret);
extern bool (*patch_osif_os_pm_store)(void);
extern bool (*patch_osif_os_pm_restore)(void);
extern bool (*patch_osif_os_register_pm_buffer)(uint8_t *pBufferStart, uint32_t length,
                                                bool shouldFree, bool *ret);
extern bool (*patch_osif_os_unregister_pm_buffer)(uint8_t *pBufferStart);
extern bool (*patch_osif_os_pm_buffer_pre_allocate)(bool *ret);
extern bool (*patch_osif_os_pm_get_buffer_store_count)(uint16_t *ret);
extern bool (*patch_osif_os_pm_buffer_store_rollback)(void);
extern bool (*patch_osif_os_pm_buffer_store)(void);
extern bool (*patch_osif_os_pm_buffer_restore)(void);
extern bool (*patch_osif_os_pm_init)(void);
extern bool (*patch_osif_os_sys_tick_rate_get)(uint32_t *ret);
extern bool (*patch_osif_os_sys_tick_clk_get)(uint32_t *ret);
extern bool (*patch_osif_os_get_sys_tick_clk_type)(uint32_t *ret);
extern bool (*patch_osif_os_sys_tick_increase)(uint32_t tick_increment, uint64_t *ret);
extern bool (*patch_osif_os_systick_handler)(void);
#ifdef __cplusplus
}
#endif

#endif /* _OS_POWER_MANAGER_H_ */
