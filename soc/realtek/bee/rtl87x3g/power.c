#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/pm/pm.h>

#include <zephyr/kernel_structs.h>
#include <zephyr/init.h>
#include <string.h>
#include <stdint.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/state.h>
#include <zephyr/pm/policy.h>
#include <zephyr/tracing/tracing.h>

#include <cmsis_core.h>

#include <trace.h>
/* for platform_pm_register_callback_func_with_priority */
#include <pm.h>
#include <os_pm.h>
#include <io_dlps.h>
#include "platform_ext.h"
#include <power_manager_unit_platform.h>

#include <zephyr/logging/log.h>

#include <rtl876x_pinmux.h>
LOG_MODULE_REGISTER(rtl87x3g_pm, LOG_LEVEL_INF);

extern void sys_clock_announce_process_timeout(void);
extern void sys_clock_restore_tick_and_cycle(void);
typedef struct
{
    uint32_t clk_div_20c;
    uint32_t peri_en_210;
    uint32_t ethernet_clk_214;
    uint32_t peri_en_218;
    uint32_t peri_en_21c;
    uint32_t clock_230;
    uint32_t clock_234;
    uint32_t clock_238;
    uint32_t clock_23c;
    uint32_t clock_244;
    uint32_t reg_pmux_sdio_ctrl_2b4;
    uint32_t clock_348;
    uint32_t clock_370;
    uint32_t clock_374;
    uint32_t clock_378;
} PERIONStoreReg_Typedef;

PERIONStoreReg_Typedef peri_on_store_buffer;
extern PERIONStoreReg_Typedef *peri_on_store_buf;

typedef struct
{
    uint32_t periph_int_reg[3];
    uint32_t cpu_reg[8];
    uint8_t  nvic_ip_reg[CONFIG_NUM_IRQS];
} CPUStoreReg_Typedef;

CPUStoreReg_Typedef cpu_store_buffer;
extern CPUStoreReg_Typedef *cpu_store_buf;

TYPE_SECTION_START_EXTERN(const struct device *, pm_device_slots);

/* Number of devices successfully suspended. */
static size_t num_susp_rtk;

extern void cpu_dlps_store(void);
extern void peri_on_store(void);

extern void peri_on_restore(void);
extern void cpu_dlps_restore(void);
extern void Pad_ClearAllWakeupINT(void);
extern void hw_timer_dlps_enter(void);
extern void hw_timer_dlps_exit(void);

void pm_state_exit_post_ops(enum pm_state state, uint8_t substate_id)
{
	ARG_UNUSED(state);
	ARG_UNUSED(substate_id);
}

static void pm_suspend_devices_rtk(void)
{
	const struct device *devs;
	size_t devc;

	Pad_ClearAllWakeupINT();

	cpu_dlps_store();
	peri_on_store();
	// pm_cpu_dlps_store();
	// pm_peri_on_store();
	// pinmux_store();
	// pad_store();
	hw_timer_dlps_enter();

	devc = z_device_get_all_static(&devs);

	num_susp_rtk = 0;

	for (const struct device *dev = devs + devc - 1; dev >= devs; dev--) {
		int ret;

		/* Ignore uninitialized devices, busy devices, wake up sources, and
		* devices with runtime PM enabled.
		*/
		if (!device_is_ready(dev) || pm_device_is_busy(dev) ||
			// pm_device_state_is_locked(dev) ||
			pm_device_wakeup_is_enabled(dev) ||
			pm_device_runtime_is_enabled(dev)) {
			continue;
		}

		ret = pm_device_action_run(dev, PM_DEVICE_ACTION_SUSPEND);
		/* ignore devices not supporting or already at the given state */
		if ((ret == -ENOSYS) || (ret == -ENOTSUP) || (ret == -EALREADY)) {
			continue;
		} else if (ret < 0) {
			LOG_ERR("Device %s did not enter %s state (%d)",
				dev->name,
				pm_device_state_str(PM_DEVICE_STATE_SUSPENDED),
				ret);
			// return ret;
		}

		TYPE_SECTION_START(pm_device_slots)[num_susp_rtk] = dev;
		num_susp_rtk++;
	}

	return;
}

void pm_resume_devices_rtk(void)
{
	Pinmux_Deinit(P3_0);
	Pinmux_Deinit(P3_1);
	// pinmux_restore();
	// pad_restore();
	peri_on_restore();
	// pm_peri_on_restore();
	hw_timer_dlps_exit();

	for (int i = (num_susp_rtk - 1); i >= 0; i--) {
		pm_device_action_run(TYPE_SECTION_START(pm_device_slots)[i],
					PM_DEVICE_ACTION_RESUME);
	}

	num_susp_rtk = 0;

    cpu_dlps_restore();
    // pm_cpu_dlps_restore();

	return;
}

void pm_reusme_systick_and_process_timeout(void)
{
	/* Restore systick after driver resume to ensure no systick isr is triggered and exclude
	 * timer is timeout out and executed. Restore the sys clock of Zephyr. Note: exclude timer
	 * cb may rely on the driver resume.
	 *
	 * !! IMPORTANT CONSTRAINT FOR PEND STAGE CALLBACKS !!
	 *
	 * This callback runs at INT8_MAX (the last priority in the pend stage), meaning SysTick
	 * and the Zephyr SW tick count are NOT updated until this point.
	 *
	 * Consequence: any pend stage callback that runs BEFORE this one (i.e. priority < INT8_MAX,
	 * including all driver resume callbacks) must NOT perform any timer-related operations,
	 * including but not limited to:
	 *   - k_timer_start() / k_timer_stop()
	 *   - k_work_schedule() / k_work_reschedule()
	 *   - k_sleep() / k_msleep()
	 *   - z_add_timeout() or any API that enqueues a timeout
	 *
	 * Reason: before this callback executes, sys_clock_tick_get() still returns the stale
	 * tick count from before the sleep.  Any timeout deadline computed as
	 * (stale_tick + delay) will appear to have already expired once the tick count is
	 * finally corrected here, causing the timer to fire immediately and unexpectedly.
	 *
	 * If a driver genuinely needs a post-resume timer, it should defer that operation to a
	 * k_work item submitted from its resume callback; the work item will run in thread context
	 * after the tick count has been restored.
	 */
	__disable_irq();
	os_pm_restore_os_tick_count();
	sys_clock_restore_tick_and_cycle();
	__enable_irq();

	/* Subtract the pended tick from the timeout list and manually trigger a timeout process. */
	sys_clock_announce_process_timeout();
}

#if defined(CONFIG_REALTEK_PM_WAKEUP_REASON_GET)
extern PMCheckResult os_pm_check(uint32_t *wakeup_time_diff);
extern const char *realtek_pm_os_timeout_type;
extern void *realtek_pm_os_timeout_handle;
extern const char *realtek_pm_os_timer_name_get(void *timer_handle);

static bool pm_wakeup_reason_is_os(PlatformWakeupReason reason)
{
	return reason == PLATFORM_PM_WAKEUP_OS || (uintptr_t)reason == (uintptr_t)os_pm_check;
}

static bool pm_wakeup_reason_is_gpio(PlatformWakeupReason reason)
{
	return reason == PLATFORM_PM_WAKEUP_GPIO;
}

static const char *pm_wakeup_reason_str(PlatformWakeupReason reason)
{
	if (pm_wakeup_reason_is_os(reason)) {
		return "PLATFORM_PM_WAKEUP_OS";
	}

	switch (reason) {
	case PLATFORM_PM_WAKEUP_UNKNOWN:
		return "PLATFORM_PM_WAKEUP_UNKNOWN";
	case PLATFORM_PM_WAKEUP_USER:
		return "PLATFORM_PM_WAKEUP_USER";
	case PLATFORM_PM_WAKEUP_PRE_SYSTEM_LEVEL:
		return "PLATFORM_PM_WAKEUP_PRE_SYSTEM_LEVEL (BT MAC Unit)";
	case PLATFORM_PM_WAKEUP_PF_RTC:
		return "PLATFORM_PM_WAKEUP_PF_RTC";
	case PLATFORM_PM_WAKEUP_RTC:
		return "PLATFORM_PM_WAKEUP_RTC";
	case PLATFORM_PM_WAKEUP_MAC:
		return "PLATFORM_PM_WAKEUP_MAC";
	case PLATFORM_PM_WAKEUP_GPIO:
		return "PLATFORM_PM_WAKEUP_GPIO";
	case PLATFORM_PM_WAKEUP_USB_RESUME:
		return "PLATFORM_PM_WAKEUP_USB_RESUME";
	case PLATFORM_PM_WAKEUP_MFB:
		return "PLATFORM_PM_WAKEUP_MFB";
	case PLATFORM_PM_WAKEUP_POW:
		return "PLATFORM_PM_WAKEUP_POW";
	case PLATFORM_PM_WAKEUP_CTC:
		return "PLATFORM_PM_WAKEUP_CTC";
	case PLATFORM_PM_WAKEUP_DEF_MAX:
		return "PLATFORM_PM_WAKEUP_DEF_MAX";
	default:
		return "PLATFORM_PM_WAKEUP_INVALID";
	}
}

static void pm_wakeup_reset_get(void)
{
	PlatformWakeupReason reason = platform_pm_get_wakeup_reason();

	LOG_INF("wakeup reason: %s", pm_wakeup_reason_str(reason));
	if (pm_wakeup_reason_is_os(reason)) {
		LOG_INF("OS timeout: type=%s, handle=0x%p",
			realtek_pm_os_timeout_type ? realtek_pm_os_timeout_type : "unknown",
			realtek_pm_os_timeout_handle);
		if (realtek_pm_os_timeout_type != NULL &&
		    strcmp(realtek_pm_os_timeout_type, "timer") == 0 &&
		    realtek_pm_os_timeout_handle != NULL) {
			const char *name = realtek_pm_os_timer_name_get(realtek_pm_os_timeout_handle);

			if (name != NULL && name[0] != '\0') {
				LOG_INF("os_timer name: %s", name);
			}
		} else if (realtek_pm_os_timeout_type != NULL &&
			   strcmp(realtek_pm_os_timeout_type, "thread") == 0 &&
			   realtek_pm_os_timeout_handle != NULL) {
			struct k_thread *thread = realtek_pm_os_timeout_handle;
			const char *name = k_thread_name_get(thread);

			LOG_INF("thread name: %s",
				(name != NULL && name[0] != '\0') ? name : "unknown");
		}
	}

	if (pm_wakeup_reason_is_gpio(reason)) {
		for (uint32_t i = 0; i < TOTAL_PIN_NUM; i++) {
			if (System_WakeupDebounceStatus(i) || System_WakeUpInterruptValue(i)) {
				LOG_INF("GPIO wakeup: pad=%d", i);
			}
		}
	}
}
#endif

/* Initialize power system */
static int rtl87x3g_power_init(void)
{
	int ret = 0;

	bt_power_mode_set(BTPOWER_DEEP_SLEEP);
	power_mode_set(POWER_DLPS_MODE);

	peri_on_store_buf = &peri_on_store_buffer;
	cpu_store_buf = &cpu_store_buffer;

#if defined(CONFIG_REALTEK_PM_WAKEUP_REASON_GET)
	platform_pm_register_callback_func_with_priority((void *)pm_wakeup_reset_get, PLATFORM_PM_PEND, INT8_MAX);
#endif
	/* Register systick restore at the lowest priority (INT8_MAX = last) in the pend stage so
	 * that all driver resume callbacks have completed before SysTick and the Zephyr SW tick
	 * count are updated.  See pm_reusme_systick_and_process_timeout() for the side-effect this
	 * ordering has on timer operations inside pend stage callbacks.
	 */
	platform_pm_register_callback_func_with_priority((void *)pm_reusme_systick_and_process_timeout, PLATFORM_PM_PEND, INT8_MAX);
	power_stage_cb_register(pm_suspend_devices_rtk, POWER_STAGE_STORE);
    power_stage_cb_register(pm_resume_devices_rtk, POWER_STAGE_RESTORE);

/* The Zephyr pend call send function incurs more time compared to FreeRTOS due to additional operations such as malloc, k_work_init, and k_work_submit.
 * Additionally, the Zephyr pend call send function runs on execute-in-place (XIP).
 * This period is not accounted for in unit_rqst.pon_stage_time, which calculates as:
 *   unit_rqst.pon_stage_time = platform_pm_system.stage_time[PLATFORM_PM_EXIT] +
 *                               platform_pm_system.stage_time[PLATFORM_PM_RESTORE];
 * Therefore, to support Realtek PM on Zephyr, we need to manually increase stage_time[PLATFORM_PM_EXIT].
 *
 * Due to the added DLPS exit actions in sys patch refered to JIRA[BB2ULTRARD-832], exit stage time needs to be increased at least to 60.
 *
 * !!Note!! The potential side effect of this change is that if more operations are added during the pend stage,
 * this stage_time might need further adjustments.
 */
/*
	In order to further reduce power consumption, compared to FreeRTOS, aligning the exit stage time to 30
	in the BLE advertising scenario can reduce power by approximately 3uA@3.7V. Meanwhile, after adjustments,
	the issue in JIRA [BB2ULTRARD-832] was retested on the B cut IC and did not recur, so the value of 30 will be used for now.
*/
	platform_pm_system.stage_time[PLATFORM_PM_EXIT] = 30;

	return ret;
}

/* do it after lowerstack entry */
SYS_INIT(rtl87x3g_power_init, APPLICATION, 1);
