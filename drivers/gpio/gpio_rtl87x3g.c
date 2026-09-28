/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_gpio

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <soc.h>

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include <rtl876x_rcc.h>
#include <rtl876x_pinmux.h>
#include <rtl876x_gpio.h>
#ifdef CONFIG_PM_DEVICE
#include "power_manager_unit_platform.h"
#endif
#endif

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/clock_control/rtl87x3g_clock_control.h>
#include <zephyr/sys/util.h>
#include <zephyr/irq.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>

#include <zephyr/dt-bindings/gpio/realtek-rtl87x3g-gpio.h>

#include "gpio_rtl87x3g.h"
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/logging/log.h>
#include "trace.h"

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#define RTL87X3G_GPIO_WriteBit(port, bit, val)            GPIO_WriteBit(port, bit, val)
#define RTL87X3G_GPIO_ReadOutputDataBit(port, bit)        GPIO_ReadOutputDataBit(port, bit)
#define RTL87X3G_GPIO_INTConfig(port, bit, val)           GPIO_INTConfig(port, bit, val)
#define RTL87X3G_GPIO_Init(port, val)                     GPIO_Init(port, val)
#define RTL87X3G_GPIO_MaskINTConfig(port, bit, val)       GPIO_MaskINTConfig(port, bit, val)
#define RTL87X3G_GPIO_ClearINTPendingBit(port, bit)       GPIO_ClearINTPendingBit(port, bit)
#define RTL87X3G_GPIO_SetBits(port, bit)                  GPIO_SetBits(port, bit)
#define RTL87X3G_GPIO_ResetBits(port, bit)                GPIO_ResetBits(port, bit)
#define RTL87X3G_GPIO_ReadInputData(port)                 GPIO_ReadInputData(port)
#define RTL87X3G_GPIO_Write(port, val)                    GPIO_Write(port, val)
#define RTL87X3G_Pad_SetControlMode(pad, mode)            Pad_ControlSelectValue(pad, mode)
#define RTL87X3G_Pad_SetOutputLevel(pad, val)             Pad_OutputControlValue(pad, val)
#define RTL87X3G_System_WakeUpPinEnable(pin, pol, deb_en) System_WakeUpPinEnable(pin, pol)
#define RTL87X3G_GPIO_REG_INTSATUS                        GPIO_INT_STS
#define RTL87X3G_GPIO_REG_INT_EN                          GPIO_INT_EN
#endif

LOG_MODULE_REGISTER(gpio_rtl87x3g, CONFIG_GPIO_LOG_LEVEL);

static int gpio_rtl87x3g_gpio2pad(uint8_t port_num, uint32_t pin)
{
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
	/* There is no reuse situation for gpioa */
	if (port_num == 0) {
		/* GPIOA0-GPIOA3 */
		if (pin < 4) {
			return pin;
			/* GPIOA4-GPIOA6 */
		} else if (pin < 7) {
			return pin + 20;
			/* GPIOA7-GPIOA10 */
		} else if (pin < 11) {
			return pin + 1;
		} else if (pin == 11) {
#if CONFIG_RTL87X3G_USE_P10_4_AS_GPIOA11
			return P10_4;
#else
			return P1_4;
#endif
		} else if (pin == 12) {
#if CONFIG_RTL87X3G_USE_P10_5_AS_GPIOA12
			return P10_5;
#else
			return P1_5;
#endif
		} else if (pin == 13) {
#if CONFIG_RTL87X3G_USE_P10_6_AS_GPIOA13
			return P10_6;
#else
			return P5_4;
#endif
		} else if (pin == 14) {
#if CONFIG_RTL87X3G_USE_SPIC3_SIO2_AS_GPIOA14
			return SPIC3_SIO2;
#else
			return P5_5;
#endif
		} else if (pin == 15) {
#if CONFIG_RTL87X3G_USE_P5_6_AS_GPIOA15
			return P5_6;
#else
			return P2_0;
#endif
		} else if (pin == 16) {
#if CONFIG_RTL87X3G_USE_SPIC3_CSN_AS_GPIOA16
			return SPIC3_CSN;
#else
			return P2_1;
#endif
		} else if (pin == 17) {
#if CONFIG_RTL87X3G_USE_SPIC3_SIO0_AS_GPIOA17
			return SPIC3_SIO0;
#else
			return P2_2;
#endif
			/* GPIOA18-GPIOA24 */
		} else if (pin < 25) {
			return pin - 1;
		} else if (pin == 25) {
#if CONFIG_RTL87X3G_USE_MIC1_P_AS_GPIOA25
			return MIC1_P;
#else
			return P6_0;
#endif
		} else if (pin == 26) {
#if CONFIG_RTL87X3G_USE_MIC1_N_AS_GPIOA26
			return MIC1_N;
#else
			return P6_1;
#endif
		} else if (pin == 27) {
#if CONFIG_RTL87X3G_USE_MIC2_P_AS_GPIOA27
			return MIC2_P;
#else
			return P6_2;
#endif
		} else if (pin == 28) {
#if CONFIG_RTL87X3G_USE_MIC2_N_AS_GPIOA28
			return MIC2_N;
#else
			return P6_3;
#endif
		} else if (pin == 29) {
#if CONFIG_RTL87X3G_USE_MICBIAS1_AS_GPIOA29
			return MICBIAS1;
#else
			return P6_4;
#endif
		} else if (pin == 30) {
#if CONFIG_RTL87X3G_USE_SPIC3_SCK_AS_GPIOA30
			return SPIC3_SCK;
#else
			return DAOUT1_P;
#endif
		} else if (pin == 31) {
#if CONFIG_RTL87X3G_USE_SPIC3_SIO3_AS_GPIOA31
			return SPIC3_SIO3;
#else
			return DAOUT1_N;
#endif
		}
	}
	/* Handle reuse situation for gpiob */
	else if (port_num == 1) {
		/* GPIOB0-GPIOB6 */
		if (pin < 7) {
			return pin + 79;
		} else if (pin == 7) {
#if CONFIG_RTL87X3G_USE_LOUT_P_AS_GPIOB7
			return LOUT_P;
#else
			return P4_0;
#endif
		} else if (pin == 8) {
#if CONFIG_RTL87X3G_USE_LOUT_N_AS_GPIOB8
			return LOUT_N;
#else
			return P4_1;
#endif
		} else if (pin == 9) {
#if CONFIG_RTL87X3G_USE_ROUT_P_AS_GPIOB9
			return ROUT_P;
#else
			return P4_2;
#endif
		} else if (pin == 10) {
#if CONFIG_RTL87X3G_USE_ROUT_N_AS_GPIOB10
			return ROUT_N;
#else
			return P4_3;
#endif
		} else if (pin == 11) {
			return P4_4;
		} else if (pin == 12) {
#if CONFIG_RTL87X3G_USE_MICBIAS2_AS_GPIOB12
			return MICBIAS2;
#else
			return P4_5;
#endif
		} else if (pin == 13) {
#if CONFIG_RTL87X3G_USE_DAOUT2_P_AS_GPIOB13
			return DAOUT2_P;
#else
			return P4_6;
#endif
		} else if (pin == 14) {
#if CONFIG_RTL87X3G_USE_DAOUT2_N_AS_GPIOB14
			return DAOUT2_N;
#else
			return P4_7;
#endif
		} else if (pin == 15) {
#if CONFIG_RTL87X3G_USE_P7_0_AS_GPIOB15
			return P7_0;
#else
			return P5_0;
#endif
		} else if (pin == 16) {
#if CONFIG_RTL87X3G_USE_P7_1_AS_GPIOB16
			return P7_1;
#else
			return P5_1;
#endif
		} else if (pin == 17) {
#if CONFIG_RTL87X3G_USE_P7_2_AS_GPIOB17
			return P7_2;
#else
			return P5_2;
#endif
		} else if (pin == 18) {
#if CONFIG_RTL87X3G_USE_P7_3_AS_GPIOB18
			return P7_3;
#else
			return P5_3;
#endif
		} else if (pin == 19) {
#if CONFIG_RTL87X3G_USE_P8_6_AS_GPIOB19
			return P8_6;
#else
			return P7_4;
#endif
		} else if (pin == 20) {
#if CONFIG_RTL87X3G_USE_P8_7_AS_GPIOB20
			return P8_7;
#else
			return P7_5;
#endif
		} else if (pin == 21) {
#if CONFIG_RTL87X3G_USE_P7_6_AS_GPIOB21
			return P7_6;
#else
			return ADC_4;
#endif
		} else if (pin == 22) {
#if CONFIG_RTL87X3G_USE_SPIC3_SIO1_AS_GPIOB22
			return SPIC3_SIO1;
#else
			return ADC_5;
#endif
		} else if (pin == 23) {
#if CONFIG_RTL87X3G_USE_SPIC1_SIO2_AS_GPIOB23
			return SPIC1_SIO2;
#else
			return ADC_6;
#endif
		} else if (pin == 24) {
#if CONFIG_RTL87X3G_USE_SPIC1_SIO1_AS_GPIOB24
			return SPIC1_SIO1;
#else
			return ADC_7;
#endif
		} else if (pin == 25) {
#if CONFIG_RTL87X3G_USE_SPIC1_CSN_AS_GPIOB25
			return SPIC1_CSN;
#else
			return P3_5;
#endif
		} else if (pin == 26) {
#if CONFIG_RTL87X3G_USE_SPIC1_SIO0_AS_GPIOB26
			return SPIC1_SIO0;
#else
			return P8_0;
#endif
		} else if (pin == 27) {
#if CONFIG_RTL87X3G_USE_SPIC1_SCK_AS_GPIOB27
			return SPIC1_SCK;
#else
			return P8_1;
#endif
		} else if (pin == 28) {
#if CONFIG_RTL87X3G_USE_SPIC1_SIO3_AS_GPIOB28
			return SPIC1_SIO3;
#else
			return P8_2;
#endif
		} else if (pin == 29) {
#if CONFIG_RTL87X3G_USE_P10_0_AS_GPIOB29
			return P10_0;
#else
			return P8_3;
#endif
		} else if (pin == 30) {
#if CONFIG_RTL87X3G_USE_P10_1_AS_GPIOB30
			return P10_1;
#else
			return P8_4;
#endif
		} else if (pin == 31) {
#if CONFIG_RTL87X3G_USE_P10_2_AS_GPIOB31
			return P10_2;
#else
			return P8_5;
#endif
		}
	}
#endif

	return -EIO;
}

static int gpio_rtl87x3g_pin_configure(const struct device *port, gpio_pin_t pin,
				       gpio_flags_t flags)
{
	LOG_DBG("port=%s, pin=%d, flags=0x%x, line%d\n", port->name, pin, flags, __LINE__);

	const struct gpio_rtl87x3g_config *config = port->config;
	struct gpio_rtl87x3g_data *data = port->data;
	GPIO_TypeDef *port_base;
	uint8_t port_num = config->port_num;
	uint32_t gpio_bit = BIT(pin);
	int pad_pin = gpio_rtl87x3g_gpio2pad(port_num, pin);
	uint32_t pull_config;
	GPIO_InitTypeDef gpio_init_struct;
	uint8_t debounce_ms = (flags & RTL87X3G_GPIO_INPUT_DEBOUNCE_MS_MASK) >>
			      RTL87X3G_GPIO_INPUT_DEBOUNCE_MS_POS;
	int ret = 0;

	port_base = config->port_base;

	__ASSERT(pad_pin >= 0, "gpio port or pin error");

	if (flags & GPIO_OPEN_SOURCE) {
		ret = -ENOTSUP;
		return ret;
	}

	if (flags == GPIO_DISCONNECTED) {
		Pinmux_Deinit(pad_pin);
		Pad_Config(pad_pin, PAD_SW_MODE, PAD_SHUTDOWN, PAD_PULL_NONE, PAD_OUT_DISABLE,
			   PAD_OUT_HIGH);
	} else {
		/* config pad pull status */

		if (flags & GPIO_PULL_UP) {
			pull_config = PAD_PULL_UP;
		} else if (flags & GPIO_PULL_DOWN) {
			pull_config = PAD_PULL_DOWN;
		} else {
			pull_config = PAD_PULL_NONE;
		}

		/* config gpio */

		GPIO_StructInit(&gpio_init_struct);

		if (debounce_ms) {
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
			gpio_init_struct.GPIO_DebounceClkSource = GPIO_DEBOUNCE_32K;
			gpio_init_struct.GPIO_DebounceClkDiv = GPIO_DEBOUNCE_DIVIDER_32;
			gpio_init_struct.GPIO_DebounceCntLimit = debounce_ms;
#endif
			gpio_init_struct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
			data->pin_debounce_ms[pin] = debounce_ms;
		} else {
			gpio_init_struct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
			data->pin_debounce_ms[pin] = 0;
		}

		gpio_init_struct.GPIO_PinBit = gpio_bit;
		gpio_init_struct.GPIO_Mode = flags & GPIO_OUTPUT ? GPIO_Mode_OUT : GPIO_Mode_IN;
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		gpio_init_struct.GPIO_OutPutMode =
			flags & GPIO_OPEN_DRAIN ? GPIO_OUTPUT_OPENDRAIN : GPIO_OUTPUT_PUSHPULL;
#endif
		gpio_init_struct.GPIO_ITCmd = flags & GPIO_INT_ENABLE ? ENABLE : DISABLE;
		gpio_init_struct.GPIO_ITTrigger =
			flags & GPIO_INT_EDGE ? GPIO_INT_Trigger_EDGE : GPIO_INT_Trigger_LEVEL;
		gpio_init_struct.GPIO_ITPolarity = flags & GPIO_INT_LOW_0
							   ? GPIO_INT_POLARITY_ACTIVE_LOW
							   : GPIO_INT_POLARITY_ACTIVE_HIGH;
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		Pad_HighSpeedMuxSel(pad_pin, FROM_AON_DOMAIN);
#endif
		Pad_Config(pad_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, pull_config,
			   flags & GPIO_OUTPUT ? PAD_OUT_ENABLE : PAD_OUT_DISABLE,
			   flags & GPIO_OUTPUT_INIT_HIGH ? PAD_OUT_HIGH : PAD_OUT_LOW);
		GPIO_SelectRemappingPAD(pad_pin);
		Pinmux_Config(pad_pin, DWGPIO);

		switch (flags & (GPIO_OUTPUT | GPIO_OUTPUT_INIT_HIGH | GPIO_OUTPUT_INIT_LOW)) {
		case (GPIO_OUTPUT_HIGH):
			RTL87X3G_GPIO_WriteBit(port_base, gpio_bit, 1);
			break;
		case (GPIO_OUTPUT_LOW):
			RTL87X3G_GPIO_WriteBit(port_base, gpio_bit, 0);
			break;
		default:
			break;
		}

		/* to avoid trigger gpio interrupt */
		if (debounce_ms && (flags & GPIO_INT_ENABLE)) {
			RTL87X3G_GPIO_INTConfig(port_base, gpio_bit, DISABLE);
			RTL87X3G_GPIO_Init(port_base, &gpio_init_struct);
			RTL87X3G_GPIO_MaskINTConfig(port_base, gpio_bit, ENABLE);
			RTL87X3G_GPIO_INTConfig(port_base, gpio_bit, ENABLE);
			k_busy_wait(data->pin_debounce_ms[pin] * 2 * 1000);
			RTL87X3G_GPIO_ClearINTPendingBit(port_base, gpio_bit);
			RTL87X3G_GPIO_MaskINTConfig(port_base, gpio_bit, DISABLE);
		} else {
			RTL87X3G_GPIO_Init(port_base, &gpio_init_struct);
		}
	}

#ifdef CONFIG_PM_DEVICE
	sys_snode_t *prev;
	if (flags & GPIO_OUTPUT) {
		data->list.array[pin].mode = PM_PAD_OUTPUT;
	} else if (flags & GPIO_INPUT) {
		if (flags & RTL87X3G_GPIO_INPUT_PM_WAKEUP) {
			data->list.array[pin].mode = PM_PAD_WAKEUP;
		} else {
			data->list.array[pin].mode = PM_PAD_INPUT;
		}
	} else {
		if (sys_slist_find(&data->list.list, (sys_snode_t *)&data->list.array[pin],
				   &prev)) {
			sys_slist_remove(&data->list.list, prev,
					 (sys_snode_t *)&data->list.array[pin]);
		}

		return 0;
	}
	if (!sys_slist_find(&data->list.list, (sys_snode_t *)&data->list.array[pin], NULL)) {
		sys_slist_append(&data->list.list, (sys_snode_t *)&data->list.array[pin]);
	}

#endif
	return 0;
}

static int gpio_rtl87x3g_port_get_raw(const struct device *port, gpio_port_value_t *value)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base;

	port_base = config->port_base;

	*value = RTL87X3G_GPIO_ReadInputData(port_base);

	return 0;
}

static int gpio_rtl87x3g_port_set_masked_raw(const struct device *port, gpio_port_pins_t mask,
					     gpio_port_value_t value)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base;

	port_base = config->port_base;

	gpio_port_pins_t pins_value = RTL87X3G_GPIO_ReadInputData(port_base);

	pins_value = (pins_value & ~mask) | (mask & value);
	RTL87X3G_GPIO_Write(port_base, pins_value);

	return 0;
}

static int gpio_rtl87x3g_port_set_bits_raw(const struct device *port, gpio_port_pins_t pins)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base;

	port_base = config->port_base;

	RTL87X3G_GPIO_SetBits(port_base, pins);

	return 0;
}

static int gpio_rtl87x3g_port_clear_bits_raw(const struct device *port, gpio_port_pins_t pins)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base;

	port_base = config->port_base;

	RTL87X3G_GPIO_ResetBits(port_base, pins);

	return 0;
}

static int gpio_rtl87x3g_port_toggle_bits(const struct device *port, gpio_port_pins_t pins)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base;

	port_base = config->port_base;

	uint32_t pins_value = RTL87X3G_GPIO_ReadInputData(port_base);

	pins_value = (pins_value | pins) & ~(pins_value & pins);
	RTL87X3G_GPIO_Write(port_base, pins_value);
	LOG_DBG("port=%s, pin=0x%x, pins_value=0x%x, line%d\n", port->name, pins, pins_value,
		__LINE__);

	return 0;
}

static int gpio_rtl87x3g_pin_interrupt_configure(const struct device *port, gpio_pin_t pin,
						 enum gpio_int_mode mode, enum gpio_int_trig trig)
{
	LOG_DBG("port=%s, pin=%d, mode=0x%x, trig=0x%x, line%d\n", port->name, pin, mode, trig,
		__LINE__);
	const struct gpio_rtl87x3g_config *config = port->config;
	struct gpio_rtl87x3g_data *data = port->data;
	GPIO_TypeDef *port_base;
	uint32_t gpio_bit = BIT(pin);
	GPIO_InitTypeDef gpio_init_struct;

	port_base = config->port_base;

#ifdef CONFIG_GPIO_ENABLE_DISABLE_INTERRUPT
	if (mode == GPIO_INT_MODE_DISABLE_ONLY) {
		RTL87X3G_GPIO_MaskINTConfig(port_base, gpio_bit, ENABLE);
		RTL87X3G_GPIO_INTConfig(port_base, gpio_bit, DISABLE);
		return 0;
	} else if (mode == GPIO_INT_MODE_ENABLE_ONLY) {
		RTL87X3G_GPIO_INTConfig(port_base, gpio_bit, ENABLE);
		RTL87X3G_GPIO_MaskINTConfig(port_base, gpio_bit, DISABLE);
		return 0;
	}
#endif /* CONFIG_GPIO_ENABLE_DISABLE_INTERRUPT */

	RTL87X3G_GPIO_INTConfig(port_base, gpio_bit, DISABLE);

	GPIO_StructInit(&gpio_init_struct);

	gpio_init_struct.GPIO_PinBit = gpio_bit;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IN;
	if (data->pin_debounce_ms[pin]) {
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		gpio_init_struct.GPIO_DebounceClkSource = GPIO_DEBOUNCE_32K;
		gpio_init_struct.GPIO_DebounceClkDiv = GPIO_DEBOUNCE_DIVIDER_32;
		gpio_init_struct.GPIO_DebounceCntLimit = data->pin_debounce_ms[pin];
#endif
		gpio_init_struct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
	} else {
		gpio_init_struct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	}

	if (mode == GPIO_INT_MODE_DISABLED) {
		return 0;
	} else if (mode == GPIO_INT_MODE_EDGE) {
		gpio_init_struct.GPIO_ITCmd = ENABLE;
		gpio_init_struct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
	} else if (mode == GPIO_INT_MODE_LEVEL) {
		gpio_init_struct.GPIO_ITCmd = ENABLE;
		gpio_init_struct.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
	}

	switch (trig) {
	case GPIO_INT_TRIG_LOW:
		gpio_init_struct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
		break;
	case GPIO_INT_TRIG_HIGH:
		gpio_init_struct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
		break;
	case GPIO_INT_TRIG_BOTH:
#if CONFIG_RTL87X3G_GPIO_SUPPORT_BOTH_EDGE
		gpio_init_struct.GPIO_ITTrigger = GPIO_INT_BOTH_EDGE;
		break;
#endif
	default:
		return -ENOTSUP;
	}

	RTL87X3G_GPIO_Init(port_base, &gpio_init_struct);
	RTL87X3G_GPIO_MaskINTConfig(port_base, gpio_bit, ENABLE);
	RTL87X3G_GPIO_INTConfig(port_base, gpio_bit, ENABLE);

	/* to avoid trigger gpio interrupt */
	if (data->pin_debounce_ms[pin]) {
		k_busy_wait(data->pin_debounce_ms[pin] * 2 * 1000);
	}

	RTL87X3G_GPIO_ClearINTPendingBit(port_base, gpio_bit);
	RTL87X3G_GPIO_MaskINTConfig(port_base, gpio_bit, DISABLE);

	return 0;
}

static int gpio_rtl87x3g_manage_callback(const struct device *port, struct gpio_callback *cb,
					 bool set)
{
	struct gpio_rtl87x3g_data *port_data = port->data;

	return gpio_manage_callback(&port_data->cb, cb, set);
}

static uint32_t gpio_rtl87x3g_get_pending_int(const struct device *dev)
{
	const struct gpio_rtl87x3g_config *config = dev->config;
	GPIO_TypeDef *port_base = config->port_base;

	return port_base->RTL87X3G_GPIO_REG_INTSATUS;
}

#ifdef CONFIG_GPIO_GET_DIRECTION
int gpio_rtl87x3g_port_get_direction(const struct device *port, gpio_port_pins_t map,
				     gpio_port_pins_t *inputs, gpio_port_pins_t *outputs)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base = config->port_base;
	gpio_port_pins_t gpio_dir_status = port_base->GPIO_DDR;

	if (inputs != NULL) {
		*inputs = gpio_dir_status;
	}

	if (outputs != NULL) {
		*outputs = ~gpio_dir_status;
	}

	return 0;
}
#endif

#ifdef CONFIG_PM_DEVICE
/*
 * A wakeup pad may use hardware debounce: after the pad wakes the SoC out of
 * DLPS, the debounced GPIO interrupt only fires once the level has been stable
 * for the debounce time. If the system re-enters DLPS before then, that
 * interrupt is lost. To avoid this, on DLPS exit we block DLPS for the debounce
 * window and release it once the owning pad's interrupt fires or the window
 * elapses.
 *
 * The state, timer and owner are shared by all GPIO ports because the DLPS
 * check callback registered with power_check_cb_register() takes no context.
 */

/* PM_CHECK_FAIL blocks DLPS; PM_CHECK_PASS allows it. Read from ISR/timer
 * context, hence volatile.
 */
static volatile PMCheckResult gpio_wakeup_debounce_state = PM_CHECK_PASS;
/* Debounce window (ms) the running timer was started with; used to decide
 * whether a later pad needs a longer window.
 */
static uint32_t gpio_wakeup_debounce_ms;
/* Port/pin that owns the running debounce timer (the pad with the longest
 * window). Only this pad's interrupt may release the DLPS block early.
 */
static uint8_t gpio_wakeup_debounce_port;
static uint8_t gpio_wakeup_debounce_gpio_num;

typedef PMCheckResult (*POWERCheckFunc)();
extern int32_t power_check_cb_register(POWERCheckFunc func);

/* DLPS gate: the PM subsystem calls this before entering DLPS and stays awake
 * while it returns PM_CHECK_FAIL.
 */
static PMCheckResult gpio_wakeup_debounce_pm_check(void)
{
	return gpio_wakeup_debounce_state;
}

/* Debounce window elapsed without the owning pad interrupting: release the
 * DLPS block so the system can sleep again.
 */
static void gpio_wakeup_debounce_timer_cb(struct k_timer *timer)
{
	k_timer_stop(timer);
	gpio_wakeup_debounce_state = PM_CHECK_PASS;
}

static K_TIMER_DEFINE(gpio_wakeup_debounce_timer, gpio_wakeup_debounce_timer_cb, NULL);

/* Starting the timer directly inside the PM resume action misbehaves: the
 * kernel tick accounting for the sleep it just left is not yet reconciled, so a
 * timer shorter than the previous sleep fires immediately. Defer the start to a
 * work item so it runs in normal thread context after ticks are reconciled.
 */
static void gpio_wakeup_debounce_work_handler(struct k_work *work)
{
	k_timer_start(&gpio_wakeup_debounce_timer, K_MSEC(gpio_wakeup_debounce_ms), K_NO_WAIT);
}

static K_WORK_DEFINE(gpio_wakeup_debounce_work, gpio_wakeup_debounce_work_handler);

static void output_pad_pm_suspend(const struct device *port, struct pm_pad_node *pad_node)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	GPIO_TypeDef *port_base = config->port_base;
	uint8_t pad_num, gpio_num;

	pad_num = pad_node->pad_num;
	gpio_num = pad_node->gpio_num;

	RTL87X3G_Pad_SetOutputLevel(pad_num,
				    RTL87X3G_GPIO_ReadOutputDataBit(port_base, BIT(gpio_num)));
	RTL87X3G_Pad_SetControlMode(pad_num, PAD_SW_MODE);
}

static void input_pad_pm_suspend(const struct device *port, struct pm_pad_node *pad_node)
{
	uint8_t pad_num;

	pad_num = pad_node->pad_num;
	RTL87X3G_Pad_SetControlMode(pad_num, PAD_SW_MODE);
}

static void wakeup_pad_pm_suspend(const struct device *port, struct pm_pad_node *pad_node)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	struct gpio_rtl87x3g_data *data = port->data;
	GPIO_TypeDef *port_base = config->port_base;
	uint8_t pad_num, gpio_num;

	pad_num = pad_node->pad_num;
	gpio_num = pad_node->gpio_num;
	if (port_base->GPIO_INT_EN & BIT(gpio_num)) {

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		bool high_trigger = port_base->GPIO_EXT_DEB_POL_CTL & BIT(gpio_num);
		bool edge_trigger = port_base->GPIO_INT_LV & BIT(gpio_num);
#endif

		if (edge_trigger) {
			pad_node->read_before_dlps =
				RTL87X3G_GPIO_ReadInputData(port_base) & BIT(gpio_num);
		}

		RTL87X3G_Pad_SetControlMode(pad_num, PAD_SW_MODE);
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		System_WakeUpPinDisable(pad_num);
#endif
		System_WakeUpDebounceEnable(pad_num);
		System_WakeUpDebounceTime(pad_num, data->pin_debounce_ms[gpio_num]);
		System_WakeUpDebounceCmd(pad_num, PAD_WAKEUP_ENABLE);
		RTL87X3G_System_WakeUpPinEnable(
			pad_num, high_trigger ? PAD_WAKEUP_POL_HIGH : PAD_WAKEUP_POL_LOW, 0);
	}
}

static void output_pad_pm_resume(const struct device *port, struct pm_pad_node *pad_node)
{
	uint8_t pad_num;

	pad_num = pad_node->pad_num;

	Pinmux_Config(pad_num, DWGPIO);
	RTL87X3G_Pad_SetControlMode(pad_num, PAD_PINMUX_MODE);
}

static void input_pad_pm_resume(const struct device *port, struct pm_pad_node *pad_node)
{
	uint8_t pad_num;

	pad_num = pad_node->pad_num;

	Pinmux_Config(pad_num, DWGPIO);
	RTL87X3G_Pad_SetControlMode(pad_num, PAD_PINMUX_MODE);
}

static void wakeup_pad_pm_resume(const struct device *port, struct pm_pad_node *pad_node)
{
	uint8_t pad_num;

	pad_num = pad_node->pad_num;

	System_WakeUpPinDisable(pad_num);
	Pinmux_Config(pad_num, DWGPIO);
	RTL87X3G_Pad_SetControlMode(pad_num, PAD_PINMUX_MODE);
}

static void wakeup_edge_trigger(const struct device *port, struct pm_pad_node *pad_node)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	struct gpio_rtl87x3g_data *data = port->data;
	GPIO_TypeDef *port_base = config->port_base;
	uint8_t pad_num, gpio_num;

	pad_num = pad_node->pad_num;
	gpio_num = pad_node->gpio_num;

	bool high_trigger = port_base->GPIO_EXT_DEB_POL_CTL & BIT(gpio_num);
	bool edge_trigger = port_base->GPIO_INT_LV & BIT(gpio_num);
	bool deb_enable = port_base->GPIO_EXT_DEB_FUNC_CTL & BIT(gpio_num);
	bool trigger_cb = false;
	bool read_after_dlps = RTL87X3G_GPIO_ReadInputData(port_base) & BIT(gpio_num);

	if (!deb_enable) {
		/* GPIO self-trigger is cleared during restore, so check the current state */
		RTL87X3G_GPIO_ClearINTPendingBit(port_base, BIT(gpio_num));
		if (edge_trigger) {
			if (high_trigger) {
				if (pad_node->read_before_dlps == 0 && read_after_dlps == 1) {
					trigger_cb = true;
				}
			} else {
				if (pad_node->read_before_dlps == 1 && read_after_dlps == 0) {
					trigger_cb = true;
				}
			}
		}

		if (trigger_cb) {
			gpio_fire_callbacks(&data->cb, port, BIT(gpio_num));
		}
	} else {
		/*
		 * Debounced wakeup pad: the debounced interrupt has not fired yet.
		 * Only arm the timer for a pad that actually woke the system (its
		 * debounce status is set, or its wakeup interrupt is pending) so
		 * unrelated debounced pads don't block DLPS.
		 */
		if (System_WakeupDebounceStatus(pad_num) || System_WakeUpInterruptValue(pad_num)) {
			uint32_t deb_ms = data->pin_debounce_ms[gpio_num] * 2;

			if (gpio_wakeup_debounce_state == PM_CHECK_PASS) {
				/* Timer idle: block DLPS and start it for this pad. */
				gpio_wakeup_debounce_state = PM_CHECK_FAIL;
				gpio_wakeup_debounce_ms = deb_ms;
				gpio_wakeup_debounce_port = config->port_num;
				gpio_wakeup_debounce_gpio_num = gpio_num;
				k_work_submit(&gpio_wakeup_debounce_work);
			} else if (deb_ms > gpio_wakeup_debounce_ms) {
				/*
				 * Timer already running for an earlier pad, but this pad
				 * needs a longer window: restart with the longer time and
				 * make this pad the new owner.
				 */
				gpio_wakeup_debounce_ms = deb_ms;
				gpio_wakeup_debounce_port = config->port_num;
				gpio_wakeup_debounce_gpio_num = gpio_num;
				k_work_submit(&gpio_wakeup_debounce_work);
			}
		}
	}
}

static int gpio_rtl87x3g_pm_action(const struct device *port, enum pm_device_action action)
{
	const struct gpio_rtl87x3g_config *config = port->config;
	struct gpio_rtl87x3g_data *data = port->data;
	GPIO_TypeDef *port_base = config->port_base;
	struct pm_pad_node *pad_node;
	uint8_t pad_num, gpio_num;

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
	extern void GPIO_DLPSEnter(void *PeriReg, void *StoreBuf);
	extern void GPIO_DLPSExitOutputMode(void *peri_reg, void *p_store_buf);
	extern void GPIO_DLPSExitInputMode(void *peri_reg, void *p_store_buf);
#endif

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:
		SYS_SLIST_FOR_EACH_CONTAINER(&data->list.list, pad_node, node) {
			pad_num = pad_node->pad_num;
			gpio_num = pad_node->gpio_num;
			switch (pad_node->mode) {
			case PM_PAD_OUTPUT:
				output_pad_pm_suspend(port, pad_node);
				break;
			case PM_PAD_INPUT:
				input_pad_pm_suspend(port, pad_node);
				break;
			case PM_PAD_WAKEUP:
				/* Enable pm wakeup function for gpios which ：
				 * 1. Configured RTL87X3G_GPIO_INPUT_PM_WAKEUP flag;
				 * 2. Enabled interrupt;
				 */
				wakeup_pad_pm_suspend(port, pad_node);
				break;
			default:
				break;
			}
		}

		GPIO_DLPSEnter(port_base, &data->store_buf);

		break;
	case PM_DEVICE_ACTION_RESUME:
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER,
				       (clock_control_subsys_t)&config->clkid);

		GPIO_DLPSExitOutputMode(port_base, &data->store_buf);
#endif

		SYS_SLIST_FOR_EACH_CONTAINER(&data->list.list, pad_node, node) {
			pad_num = pad_node->pad_num;
			gpio_num = pad_node->gpio_num;
			switch (pad_node->mode) {
			case PM_PAD_OUTPUT:
				output_pad_pm_resume(port, pad_node);
				break;
			case PM_PAD_INPUT:
				input_pad_pm_resume(port, pad_node);
				break;
			case PM_PAD_WAKEUP:
				wakeup_pad_pm_resume(port, pad_node);
				break;
			default:
				break;
			}
		}

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		GPIO_DLPSExitInputMode(port_base, &data->store_buf);
#endif
		SYS_SLIST_FOR_EACH_CONTAINER(&data->list.list, pad_node, node) {
			pad_num = pad_node->pad_num;
			gpio_num = pad_node->gpio_num;
			switch (pad_node->mode) {
			case PM_PAD_INPUT:
			case PM_PAD_WAKEUP:
				wakeup_edge_trigger(port, pad_node);
				break;
			default:
				break;
			}
		}

		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}
#endif /* CONFIG_PM_DEVICE */

static const struct gpio_driver_api gpio_rtl87x3g_driver_api = {
	.pin_configure = gpio_rtl87x3g_pin_configure,
	.port_get_raw = gpio_rtl87x3g_port_get_raw,
	.port_set_masked_raw = gpio_rtl87x3g_port_set_masked_raw,
	.port_set_bits_raw = gpio_rtl87x3g_port_set_bits_raw,
	.port_clear_bits_raw = gpio_rtl87x3g_port_clear_bits_raw,
	.port_toggle_bits = gpio_rtl87x3g_port_toggle_bits,
	.pin_interrupt_configure = gpio_rtl87x3g_pin_interrupt_configure,
	.manage_callback = gpio_rtl87x3g_manage_callback,
	.get_pending_int = gpio_rtl87x3g_get_pending_int,
#ifdef CONFIG_GPIO_GET_DIRECTION
	.port_get_direction = gpio_rtl87x3g_port_get_direction,
#endif
};

static void gpio_rtl87x3g_isr(void *arg)
{
	LOG_DBG("line%d\n", __LINE__);
	const struct device *dev = (struct device *)arg;
	const struct gpio_rtl87x3g_config *config = dev->config;
	struct gpio_rtl87x3g_data *data = dev->data;
	GPIO_TypeDef *port_base = config->port_base;
	const struct device *port = dev;
	uint32_t pins = port_base->RTL87X3G_GPIO_REG_INTSATUS;

	gpio_fire_callbacks(&data->cb, port, pins);

	for (uint32_t i = 0; i < 32; i++) {
		if (BIT(i) & pins) {
			RTL87X3G_GPIO_ClearINTPendingBit(port_base, BIT(i) & pins);
		}
	}

#if defined(CONFIG_PM_DEVICE)
	/*
	 * The ISR may fire for a different pad than the one that armed the timer.
	 * Only release the DLPS block when the owning pad (the longest-debounce
	 * pad) actually interrupts; other pads leave the timer running. Timer
	 * expiry releases the block via the timer callback.
	 */
	if (gpio_wakeup_debounce_state == PM_CHECK_FAIL &&
	    config->port_num == gpio_wakeup_debounce_port &&
	    (pins & BIT(gpio_wakeup_debounce_gpio_num))) {
		k_timer_stop(&gpio_wakeup_debounce_timer);
		gpio_wakeup_debounce_state = PM_CHECK_PASS;
	}
#endif
}

/**
 * @brief Initialize GPIO port
 *
 * Perform basic initialization of a GPIO port. The code will
 * enable the clock for corresponding peripheral.
 *
 * @param dev GPIO device struct
 *
 * @return 0
 */
static int gpio_rtl87x3g_init(const struct device *dev)
{
	struct gpio_rtl87x3g_data *data = dev->data;
	const struct gpio_rtl87x3g_config *config = dev->config;
	int ret = 0;

	(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER, (clock_control_subsys_t)&config->clkid);

	for (uint8_t i = 0; i < config->irq_info->num_irq; ++i) {
		irq_connect_dynamic(config->irq_info->gpio_irqs[i].irq,
				    config->irq_info->gpio_irqs[i].priority,
				    (const void *)gpio_rtl87x3g_isr, dev, 0);
		irq_enable(config->irq_info->gpio_irqs[i].irq);
	}

	data->dev = dev;
	memset(data->pin_debounce_ms, 0, sizeof(data->pin_debounce_ms));

#ifdef CONFIG_PM_DEVICE
	sys_slist_init(&(data->list.list));
	for (uint8_t i = 0; i < 32; i++) {
		data->list.array[i].gpio_num = i;
		data->list.array[i].pad_num = gpio_rtl87x3g_gpio2pad(config->port_num, i);
	}

	/* The DLPS check and its timer are shared by all ports, so register once. */
	static bool pm_check_registered;

	if (!pm_check_registered) {
		power_check_cb_register(gpio_wakeup_debounce_pm_check);
		pm_check_registered = true;
	}
#endif
	return ret;
}

#define GPIO_RTL87X3G_SET_GPIO_IRQ_INFO(irq_idx, index)                                            \
	{                                                                                          \
		.irq = DT_INST_IRQ_BY_IDX(index, irq_idx, irq),                                    \
		.priority = DT_INST_IRQ_BY_IDX(index, irq_idx, priority),                          \
	}

#define GPIO_RTL87X3G_SET_IRQ_INFO(index)                                                          \
	static struct gpio_rtl87x3g_irq_info gpio_rtl87x3g_irq_info##index = {                     \
		.gpio_irqs = {LISTIFY(DT_NUM_IRQS(DT_DRV_INST(index)),                             \
				      GPIO_RTL87X3G_SET_GPIO_IRQ_INFO, (, ), index)},              \
		.num_irq = DT_NUM_IRQS(DT_DRV_INST(index))};

#define GPIO_RTL87X3G_GET_IRQ_INFO(index) .irq_info = &gpio_rtl87x3g_irq_info##index,

#ifdef CONFIG_PM_DEVICE
#define GPIO_RTL87X3G_ARRAY_DEFINE(index) struct pm_pad_node pm_pad_node_array##index[32];

#define GPIO_RTL87X3G_DATA_INIT(index) .list.array = pm_pad_node_array##index,

#else
#define GPIO_RTL87X3G_ARRAY_DEFINE(index)
#define GPIO_RTL87X3G_DATA_INIT(index)
#endif

#define GPIO_RTL87X3G_DEVICE_INIT(index)                                                           \
	GPIO_RTL87X3G_ARRAY_DEFINE(index)                                                          \
	GPIO_RTL87X3G_SET_IRQ_INFO(index)                                                          \
	static const struct gpio_rtl87x3g_config gpio_rtl87x3g_port##index##_cfg = {               \
		.common =                                                                          \
			{                                                                          \
				.port_pin_mask = GPIO_PORT_PIN_MASK_FROM_DT_INST(index),           \
			},                                                                         \
		.port_num = DT_INST_PROP(index, port),                                             \
		.port_base = (GPIO_TypeDef *)DT_INST_REG_ADDR(index),                              \
		.clkid = DT_INST_CLOCKS_CELL(index, id),                                           \
		GPIO_RTL87X3G_GET_IRQ_INFO(index)};                                                \
                                                                                                   \
	static struct gpio_rtl87x3g_data gpio_rtl87x3g_port##index##_data = {                      \
		GPIO_RTL87X3G_DATA_INIT(index)};                                                   \
	PM_DEVICE_DT_INST_DEFINE(index, gpio_rtl87x3g_pm_action);                                  \
	DEVICE_DT_INST_DEFINE(index, gpio_rtl87x3g_init, PM_DEVICE_DT_INST_GET(index),             \
			      &gpio_rtl87x3g_port##index##_data, &gpio_rtl87x3g_port##index##_cfg, \
			      PRE_KERNEL_1, CONFIG_GPIO_INIT_PRIORITY, &gpio_rtl87x3g_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_RTL87X3G_DEVICE_INIT)
