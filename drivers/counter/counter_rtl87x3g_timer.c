/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_timer

#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/rtl87x3g_clock_control.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/irq.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <soc.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>

#include "trace.h"
#include <hw_tim.h>
#include <rtl876x_tim.h>
#include <rtl876x_rcc.h>
#include "rtl876x_rtc_def.h"

LOG_MODULE_REGISTER(counter_rtl87x3g_timer, CONFIG_COUNTER_LOG_LEVEL);

struct counter_rtl87x3g_ch_data {
	counter_alarm_callback_t callback;
	void *user_data;
};

#ifdef CONFIG_PM_DEVICE
#define US_TO_TICK_THRESHOLD (0x3FFFFFFF)
#define US_TO_PF_RTC_TICK(us)                                                                      \
	(((us) > US_TO_TICK_THRESHOLD) ? (((us) / 125) << 2) : (((us) << 2) / 125))
#define PF_RTC_GET_CURRENT_COUNT()          HAL_READ32(RTC_REG_BASE, 0x58)
#define PF_RTC_CNT_TO_US_OVERFLOW_THRESHOLD (0x83126E9)
#define PF_RTC_TICK_TO_US(count)                                                                   \
	(((count) > PF_RTC_CNT_TO_US_OVERFLOW_THRESHOLD) ? 0xFFFFFFFF                              \
							 : (((count) * 31) + ((count) >> 2)))
#define RTC_INTERVAL_CAL(before_cnt, after_cnt)                                                    \
	(((before_cnt) > (after_cnt)) ? (0xFFFFFFFFUL - (before_cnt) + (after_cnt))                \
				      : ((after_cnt) - (before_cnt)))
#define DLPS_THRESHOLD_TIME_USEC 30000

typedef struct {
	uint32_t timer_reg[11];
} TIMERStoreReg_Typedef;

struct pm_counter_node {
	sys_snode_t node;
	const struct device *dev;
};
#endif

struct counter_rtl87x3g_data {
	counter_top_callback_t top_cb;
	void *top_user_data;
	uint32_t guard_period;
	uint32_t freq;
	bool started;
	bool pwm_used;
#ifdef CONFIG_PM_DEVICE
	struct pm_counter_node pm_node;
	TIMERStoreReg_Typedef store_buf;
	uint32_t pf_rtc_dlps_store_cnt;
#endif
	struct counter_rtl87x3g_ch_data alarm[];
};

struct counter_rtl87x3g_config {
	struct counter_config_info counter_info;
	uint32_t reg;
	uint16_t clkid;
	uint8_t prescaler;
	void (*irq_config)(const struct device *dev);
	uint32_t (*get_irq_pending)(void);
};

#ifdef CONFIG_PM_DEVICE
sys_slist_t pm_counter_list;
uint32_t min_timeout_cnt;
typedef bool (*POWERCheckFunc)();
extern int32_t power_check_cb_register(POWERCheckFunc func);

static uint32_t cnt2usec(const struct device *dev, uint32_t cnt)
{
	struct counter_rtl87x3g_data *data = dev->data;
	return cnt / (data->freq / USEC_PER_SEC);
}

static bool pm_device_counter_check(uint32_t *p_next_wake_up_time)
{
	struct pm_counter_node *cur_node;
	const struct device *dev, *min_timeout_dev = NULL;
	const struct counter_rtl87x3g_config *cfg;
	uint32_t cur_cnt;
	uint32_t min_us;

	min_timeout_cnt = UINT32_MAX;

	SYS_SLIST_FOR_EACH_CONTAINER(&pm_counter_list, cur_node, node) {
		dev = cur_node->dev;
		cfg = dev->config;
		cur_cnt = TIM_GetCurrentValue((TIM_TypeDef *)cfg->reg);
		if (cnt2usec(dev, cur_cnt) < DLPS_THRESHOLD_TIME_USEC) {
			return false;
		}

		if (cur_cnt < min_timeout_cnt) {
			min_timeout_cnt = cur_cnt;
			min_timeout_dev = dev;
		}
	}

	if (min_timeout_cnt != UINT32_MAX) {
		min_us = cnt2usec(min_timeout_dev, min_timeout_cnt);
		/* TODO set pf rtc to wakeup??? */
		*p_next_wake_up_time = US_TO_PF_RTC_TICK(min_us);
	}

	return true;
}
#endif

static int counter_rtl87x3g_timer_start(const struct device *dev)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;
	struct counter_rtl87x3g_data *data = dev->data;

	LOG_DBG("[%s] %s, ctrl=0x%x, current=0x%x, line%d\n", __func__, dev->name,
		((TIM_TypeDef *)cfg->reg)->TIMER_MODE_CFG,
		TIM_GetCurrentValue((TIM_TypeDef *)cfg->reg), __LINE__);

	if (!data->started) {
#ifdef CONFIG_PM_DEVICE
		sys_slist_append(&pm_counter_list, &data->pm_node.node);
#endif
		data->started = true;
	}
	TIM_Cmd((TIM_TypeDef *)cfg->reg, ENABLE);

	return 0;
}

static int counter_rtl87x3g_timer_stop(const struct device *dev)
{
	LOG_DBG("[%s] %s, counter stop\n", __func__, dev->name);
	const struct counter_rtl87x3g_config *cfg = dev->config;
	struct counter_rtl87x3g_data *data = dev->data;

	TIM_Cmd((TIM_TypeDef *)cfg->reg, DISABLE);
#ifdef CONFIG_PM_DEVICE
	sys_slist_find_and_remove(&pm_counter_list, &data->pm_node.node);
#endif
	data->started = false;

	return 0;
}

static int counter_rtl87x3g_timer_get_value(const struct device *dev, uint32_t *ticks)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;

	*ticks = TIM_GetCurrentValue((TIM_TypeDef *)cfg->reg);

	LOG_DBG("[%s] %s, ticks=0x%x\n", __func__, dev->name, *ticks);

	return 0;
}

static uint32_t counter_rtl87x3g_timer_get_top_value(const struct device *dev)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;

	LOG_DBG("[%s] %s, topvalue=0x%x\n", __func__, dev->name,
		((TIM_TypeDef *)cfg->reg)->TIMER_MAX_CNT);
	return ((TIM_TypeDef *)cfg->reg)->TIMER_MAX_CNT;
}

static int counter_rtl87x3g_timer_set_alarm(const struct device *dev, uint8_t chan,
					    const struct counter_alarm_cfg *alarm_cfg)
{
	struct counter_rtl87x3g_data *data = dev->data;
	struct counter_rtl87x3g_ch_data *chdata = &data->alarm[chan];

	if (alarm_cfg->ticks > counter_rtl87x3g_timer_get_top_value(dev)) {
		return -EINVAL;
	}

	if (chdata->callback) {
		return -EBUSY;
	}

	chdata->callback = alarm_cfg->callback;
	chdata->user_data = alarm_cfg->user_data;

	LOG_DBG("[%s] %s, chan=%d\n", __func__, dev->name, chan);
	LOG_ERR("Unspported alarm");
	return -ENOTSUP;
}

static int counter_rtl87x3g_timer_cancel_alarm(const struct device *dev, uint8_t chan)
{
	struct counter_rtl87x3g_data *data = dev->data;

	data->alarm[chan].callback = NULL;

	LOG_DBG("[%s] %s, chan=%d\n", __func__, dev->name, chan);
	LOG_ERR("Unspported alarm");
	return -ENOTSUP;
}

static int counter_rtl87x3g_timer_set_top_value(const struct device *dev,
						const struct counter_top_cfg *top_cfg)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;
	struct counter_rtl87x3g_data *data = dev->data;
	int err = 0;

	for (uint32_t i = 0; i < cfg->counter_info.channels; i++) {
		/* Overflow can be changed only when all alarms are
		 * disables.
		 */
		if (data->alarm[i].callback) {
			return -EBUSY;
		}
	}
	LOG_DBG("[%s] %s, ticks=0x%x, flags=0x%x, line%d\n", __func__, dev->name, top_cfg->ticks,
		top_cfg->flags, __LINE__);

	TIM_INTConfig((TIM_TypeDef *)cfg->reg, DISABLE);
	TIM_ChangePeriod((TIM_TypeDef *)cfg->reg, top_cfg->ticks);
	TIM_ClearINT((TIM_TypeDef *)cfg->reg);
	data->top_cb = top_cfg->callback;
	data->top_user_data = top_cfg->user_data;
	if ((!(top_cfg->flags & COUNTER_TOP_CFG_DONT_RESET)) ||
	    (top_cfg->flags & COUNTER_TOP_CFG_RESET_WHEN_LATE)) {
		if (!data->started) {
#ifdef CONFIG_PM_DEVICE
			sys_slist_append(&pm_counter_list, &data->pm_node.node);
#endif
			data->started = true;
		}
		TIM_Cmd((TIM_TypeDef *)cfg->reg, DISABLE);
		TIM_Cmd((TIM_TypeDef *)cfg->reg, ENABLE);
	}

	if (top_cfg->callback) {
		TIM_INTConfig((TIM_TypeDef *)cfg->reg, ENABLE);
	}

	return err;
}

static uint32_t counter_rtl87x3g_timer_get_pending_int(const struct device *dev)
{
	LOG_DBG("get pending\n");

	const struct counter_rtl87x3g_config *cfg = dev->config;
	return cfg->get_irq_pending();
}

static uint32_t counter_rtl87x3g_timer_get_freq(const struct device *dev)
{
	struct counter_rtl87x3g_data *data = dev->data;
	LOG_DBG("freq=%d\n", data->freq);

	return data->freq;
}

static uint32_t counter_rtl87x3g_timer_get_guard_period(const struct device *dev, uint32_t flags)
{
	LOG_DBG("get guard period\n");

	struct counter_rtl87x3g_data *data = dev->data;

	return data->guard_period;
}

static int counter_rtl87x3g_timer_set_guard_period(const struct device *dev, uint32_t guard,
						   uint32_t flags)
{
	LOG_DBG("guard=%d\n", guard);
	struct counter_rtl87x3g_data *data = dev->data;

	__ASSERT_NO_MSG(guard < counter_rtl87x3g_timer_get_top_value(dev));

	data->guard_period = guard;
	return 0;
}

static void top_irq_handle(const struct device *dev)
{
	struct counter_rtl87x3g_data *data = dev->data;
	counter_top_callback_t cb = data->top_cb;
	const struct counter_rtl87x3g_config *cfg = dev->config;

	if (TIM_GetINTStatus((TIM_TypeDef *)cfg->reg)) {
		TIM_ClearINT((TIM_TypeDef *)cfg->reg);
		__ASSERT(cb != NULL, "top event enabled - expecting callback");
		cb(dev, data->top_user_data);
	}
}

static void alarm_irq_handle(const struct device *dev, uint32_t chan)
{
	LOG_DBG("chan=%d\n", chan);
	struct counter_rtl87x3g_data *data = dev->data;
	struct counter_rtl87x3g_ch_data *alarm = &data->alarm[chan];
	counter_alarm_callback_t cb;
	uint32_t ticks;
	counter_rtl87x3g_timer_get_value(dev, &ticks);
	cb = alarm->callback;
	alarm->callback = NULL;

	if (cb) {
		cb(dev, chan, ticks, alarm->user_data);
	}
}

#ifdef CONFIG_PM_DEVICE
static void TIMER_DLPSEnter(void *PeriReg, void *StoreBuf)
{
	TIMERStoreReg_Typedef *store_buf = (TIMERStoreReg_Typedef *)StoreBuf;
	TIM_TypeDef *TIMx = (TIM_TypeDef *)PeriReg;

	store_buf->timer_reg[0] = TIMx->TIMER_CUR_CNT;
	store_buf->timer_reg[1] = TIMx->TIMER_MODE_CFG;
	store_buf->timer_reg[2] = TIMx->TIMER_MAX_CNT;
	store_buf->timer_reg[3] = TIMx->TIMER_CCR;
	store_buf->timer_reg[4] = TIMx->TIMER_CCR_FIFO;
	store_buf->timer_reg[5] = TIMx->TIMER_PWM_CFG;
	store_buf->timer_reg[6] = TIMx->TIMER_PWM_SHIFT_CNT;
	store_buf->timer_reg[7] = TIMx->TIMER_DMA_CFG;
	store_buf->timer_reg[8] = TIMER1_SHARE->TIMER_EN_CTRL;
	store_buf->timer_reg[9] = TIMER1_SHARE->TIMER_INTR_EN_CTRL;
	store_buf->timer_reg[10] = TIMER1_SHARE->TIMER_PAUSE_INTR_EN_CTRL;
}

static void TIMER_DLPSExit(void *PeriReg, void *StoreBuf, uint32_t interval_cnt)
{
	TIMERStoreReg_Typedef *store_buf = (TIMERStoreReg_Typedef *)StoreBuf;
	TIM_TypeDef *TIMx = (TIM_TypeDef *)PeriReg;
	uint8_t id = ((uint32_t)TIMx - TIMER1_BASE) / 0X50;
	uint32_t load_count_setting = store_buf->timer_reg[0] > (interval_cnt)
					      ? (store_buf->timer_reg[0] - (interval_cnt))
					      : 5;

	TIMx->TIMER_MODE_CFG = store_buf->timer_reg[1];
	TIMx->TIMER_MAX_CNT = load_count_setting;
	if ((void *)TIMx >= (void *)TIM1_CH4) {
		TIMx->TIMER_PWM_CFG = store_buf->timer_reg[5];
		TIMx->TIMER_CCR = store_buf->timer_reg[3];
	}
	if ((void *)TIMx >= (void *)TIM1_CH6) {
		TIMx->TIMER_CCR_FIFO = store_buf->timer_reg[4];
		TIMx->TIMER_PWM_SHIFT_CNT = store_buf->timer_reg[6];
		TIMx->TIMER_DMA_CFG = store_buf->timer_reg[7];
	}

	TIMER1_SHARE->TIMER_PAUSE_INTR_EN_CTRL |= (store_buf->timer_reg[10] & BIT(id));
	TIMER1_SHARE->TIMER_INTR_EN_CTRL |= (store_buf->timer_reg[9] & BIT(id));
	TIMER1_SHARE->TIMER_EN_CTRL |= (store_buf->timer_reg[8] & BIT(id));
	// if (hw_rtk_timer_is_one_shot(TIMx)) {
	// 	TIMER1_SHARE->TIMER_ONESHOT_GO_CTRL |= BIT(id);
	// }
	while (TIMx->TIMER_CUR_CNT > load_count_setting)
		;

	TIMx->TIMER_MAX_CNT = store_buf->timer_reg[2];
}

static int counter_rtl87x3g_timer_pm_action(const struct device *dev, enum pm_device_action action)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;
	struct counter_rtl87x3g_data *data = dev->data;
	void *timer_base = (void *)cfg->reg;

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:
		if (((TIM_TypeDef *)timer_base)->TIMER_PWM_CFG & BIT0) {
			data->pwm_used = 1;
			break;
		}
		TIMER_DLPSEnter(timer_base, &data->store_buf);
		data->pf_rtc_dlps_store_cnt = PF_RTC_GET_CURRENT_COUNT();
		break;
	case PM_DEVICE_ACTION_RESUME:
		if (data->pwm_used) {
			break;
		}
		(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER,
				       (clock_control_subsys_t)&cfg->clkid);
		uint32_t current_pf_rtc_count = PF_RTC_GET_CURRENT_COUNT();
		uint32_t total_interval_us = PF_RTC_TICK_TO_US(
			RTC_INTERVAL_CAL(data->pf_rtc_dlps_store_cnt, current_pf_rtc_count));
		uint32_t total_interval_cnt = total_interval_us * (data->freq / USEC_PER_SEC);
		TIMER_DLPSExit(timer_base, &data->store_buf, total_interval_cnt);
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}
#endif /* CONFIG_PM_DEVICE */

static void irq_handler(const struct device *dev)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;
	top_irq_handle(dev);

	for (uint32_t i = 0; i < cfg->counter_info.channels; i++) {
		alarm_irq_handle(dev, i);
	}
}

static int counter_rtl87x3g_timer_init(const struct device *dev)
{
	const struct counter_rtl87x3g_config *cfg = dev->config;
	struct counter_rtl87x3g_data *data = dev->data;
	void *timer_base = (void *)cfg->reg;
	uint8_t clock_div;

	/* use clock_control_get_rate if clock driver is avaliable */
	uint32_t pclk = 40000000;

	(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER, (clock_control_subsys_t)&cfg->clkid);

	data->freq = pclk / cfg->prescaler;

	cfg->irq_config(dev);

	switch (cfg->prescaler) {
	case 1:
		clock_div = TIM_CLOCK_DIVIDER_1;
		break;
	case 2:
		clock_div = TIM_CLOCK_DIVIDER_2;
		break;
	case 4:
		clock_div = TIM_CLOCK_DIVIDER_4;
		break;
	case 8:
		clock_div = TIM_CLOCK_DIVIDER_8;
		break;
	case 16:
		clock_div = TIM_CLOCK_DIVIDER_16;
		break;
	case 32:
		clock_div = TIM_CLOCK_DIVIDER_32;
		break;
	case 40:
		clock_div = TIM_CLOCK_DIVIDER_40;
		break;
	case 64:
		clock_div = TIM_CLOCK_DIVIDER_64;
		break;
	default:
		clock_div = TIM_CLOCK_DIVIDER_1;
		break;
	}

	TIM_TimeBaseInitTypeDef timer_init_struct;
	TIM_StructInit(&timer_init_struct);
	timer_init_struct.TIM_ClockSrc = CK_40M_TIMER;
	timer_init_struct.TIM_ClockDiv = clock_div;
	timer_init_struct.TIM_Mode = TIM_Mode_UserDefine;
	timer_init_struct.TIM_Period = cfg->counter_info.max_top_value;
	TIM_TimeBaseInit((TIM_TypeDef *)timer_base, &timer_init_struct);
	LOG_DBG("[%s] %s, ctrl=%x, line%d\n", __func__, dev->name,
		((TIM_TypeDef *)timer_base)->TIMER_MODE_CFG, __LINE__);

#ifdef CONFIG_PM_DEVICE
	power_check_cb_register(pm_device_counter_check);
	sys_slist_init(&pm_counter_list);
	data->pm_node.dev = dev;
#endif

	return 0;
}

static const struct counter_driver_api counter_rtl87x3g_timer_driver_api = {
	.start = counter_rtl87x3g_timer_start,
	.stop = counter_rtl87x3g_timer_stop,
	.get_value = counter_rtl87x3g_timer_get_value,
	.set_alarm = counter_rtl87x3g_timer_set_alarm,
	.cancel_alarm = counter_rtl87x3g_timer_cancel_alarm,
	.set_top_value = counter_rtl87x3g_timer_set_top_value,
	.get_pending_int = counter_rtl87x3g_timer_get_pending_int,
	.get_top_value = counter_rtl87x3g_timer_get_top_value,
	.get_guard_period = counter_rtl87x3g_timer_get_guard_period,
	.set_guard_period = counter_rtl87x3g_timer_set_guard_period,
	.get_freq = counter_rtl87x3g_timer_get_freq,
};

#define TIMER_IRQ_CONFIG(index)                                                                    \
	static void irq_config_##index(const struct device *dev)                                   \
	{                                                                                          \
		IRQ_CONNECT(DT_INST_IRQN(index), DT_INST_IRQ(index, priority), irq_handler,        \
			    DEVICE_DT_INST_GET(index), 0);                                         \
		irq_enable(DT_INST_IRQN(index));                                                   \
	}                                                                                          \
	static uint32_t get_irq_pending_##index(void)                                              \
	{                                                                                          \
		return NVIC_GetPendingIRQ(DT_INST_IRQN(index));                                    \
	}

#define RTL87X3G_TIMER_INIT(index)                                                                 \
	TIMER_IRQ_CONFIG(index);                                                                   \
	static struct timer_data_##index {                                                         \
		struct counter_rtl87x3g_data data;                                                 \
		struct counter_rtl87x3g_ch_data alarm[DT_INST_PROP(index, channels)];              \
	} counter_rtl87x3g_data_##index = {0};                                                     \
	static const struct counter_rtl87x3g_config counter_rtl87x3g_config_##index = {            \
		.counter_info =                                                                    \
			{                                                                          \
				.max_top_value = UINT32_MAX,                                       \
				.flags = 0,                                                        \
				.freq = 0,                                                         \
				.channels = DT_INST_PROP(index, channels),                         \
			},                                                                         \
		.reg = DT_INST_REG_ADDR(index),                                                    \
		.clkid = DT_INST_CLOCKS_CELL(index, id),                                           \
		.prescaler = DT_INST_PROP(index, prescaler),                                       \
		.irq_config = irq_config_##index,                                                  \
		.get_irq_pending = get_irq_pending_##index,                                        \
	};                                                                                         \
                                                                                                   \
	PM_DEVICE_DT_INST_DEFINE(index, counter_rtl87x3g_timer_pm_action);                         \
	DEVICE_DT_INST_DEFINE(index, counter_rtl87x3g_timer_init, PM_DEVICE_DT_INST_GET(index),    \
			      &counter_rtl87x3g_data_##index, &counter_rtl87x3g_config_##index,    \
			      PRE_KERNEL_1, CONFIG_COUNTER_INIT_PRIORITY,                          \
			      &counter_rtl87x3g_timer_driver_api);

DT_INST_FOREACH_STATUS_OKAY(RTL87X3G_TIMER_INIT);
