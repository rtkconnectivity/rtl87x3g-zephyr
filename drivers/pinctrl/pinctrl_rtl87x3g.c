/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/pinctrl.h>

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include <rtl876x_pinmux.h>
#endif

#include <trace.h>
#define DBG_DIRECT_SHOW 0

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
static PAD_FUNCTION_CONFIG_VAL led_function[] = {LED0, LED1, LED2, LP_PWM};

#define rtl87x3g_pad_set_pull(pin, stre)       Pad_PullConfigValue(pin, stre)
#define rtl87x3g_pad_wakeup(pin, pol, en, deb) System_WakeUpPinEnable(pin, pol)
#endif

static void pinctrl_configure_pin(const pinctrl_soc_pin_t *pin)
{
	uint32_t cfg_fun = pin[0].fun;
	uint32_t cfg_pin = pin[0].pin;
	uint32_t cfg_dir = pin[0].dir;
	uint32_t cfg_drv = pin[0].drive;
	uint32_t cfg_pull = pin[0].pull;
	uint32_t cfg_pull_strength = pin[0].pull_strength;
	uint32_t cfg_wakeup_high = pin[0].wakeup_high;
	uint32_t cfg_wakeup_low = pin[0].wakeup_low;
	uint32_t current_level = pin[0].current_level;

#if DBG_DIRECT_SHOW
	DBG_DIRECT("[%s] cfg_fun=%d, cfg_pin=%d, cfg_dir=%d,"
		   " cfg_drv=%d , cfg_pull=%d, cfg_pull_strength=%d, cfg_wakeup_high=%d, "
		   "cfg_wakeup_low=%d",
		   __func__, cfg_fun, cfg_pin, cfg_dir, cfg_drv, cfg_pull, cfg_pull_strength,
		   cfg_wakeup_high, cfg_wakeup_low);
#endif

	rtl87x3g_pad_set_pull(cfg_pin, cfg_pull_strength);

	switch (current_level) {
	case 0:
		Pad_SetPinDrivingCurrent(cfg_pin, PAD_DRIVING_LEVEL0);
		break;

	case 1:
		Pad_SetPinDrivingCurrent(cfg_pin, PAD_DRIVING_LEVEL1);
		break;

	case 2:
		Pad_SetPinDrivingCurrent(cfg_pin, PAD_DRIVING_LEVEL2);
		break;

	case 3:
		Pad_SetPinDrivingCurrent(cfg_pin, PAD_DRIVING_LEVEL3);
		break;

	default:
		break;
	}

	Pinmux_Deinit(cfg_pin);

	if (cfg_fun == RTL87X3G_PWR_OFF) {
		/* shut down mode */
		Pad_Config(cfg_pin, PAD_SW_MODE, PAD_SHUTDOWN, cfg_pull, cfg_dir, cfg_drv);
	} else if (cfg_fun == RTL87X3G_SW_MODE) {
		/* sw mode */
		Pad_Config(cfg_pin, PAD_SW_MODE, PAD_IS_PWRON, cfg_pull, cfg_dir, cfg_drv);
		Pad_HighSpeedMuxSel(cfg_pin, FROM_AON_DOMAIN);
	} else if (cfg_fun < RTL87X3G_PINMUX_MAX) {
		/* pinmux mode */
		Pad_Config(cfg_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, cfg_pull, cfg_dir, cfg_drv);
		Pinmux_Config(cfg_pin, cfg_fun);
		Pad_HighSpeedMuxSel(cfg_pin, FROM_AON_DOMAIN);
	} else if (cfg_fun < RTL87X3G_SW_MODE) {
		/* sleep led mode */
		Pad_Config(cfg_pin, PAD_SW_MODE, PAD_IS_PWRON, cfg_pull, cfg_dir, cfg_drv);
		Pad_HighSpeedMuxSel(cfg_pin, FROM_AON_DOMAIN);
		Pad_FunctionConfig(cfg_pin, led_function[cfg_fun - RTL87X3G_LED0]);
	} else {
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
		/* hs mode */
		Pad_HighSpeedMuxSel(cfg_pin, FROM_CORE_DOMAIN);
		Pad_HighSpeedFuncSel(cfg_pin, cfg_fun & BIT(0));
		Pad_Config(cfg_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, cfg_pull, cfg_dir, cfg_drv);
		Pinmux_Deinit(cfg_pin);
		*((uint32_t *)0x400002ac) |= (0xf << 19);
#endif
	}

	if (!cfg_wakeup_high && !cfg_wakeup_low) {
		System_WakeUpPinDisable(cfg_pin);
	} else if (cfg_wakeup_high) {
		rtl87x3g_pad_wakeup(cfg_pin, PAD_WAKEUP_POL_HIGH, DISABLE, 0);
	} else if (cfg_wakeup_low) {
		rtl87x3g_pad_wakeup(cfg_pin, PAD_WAKEUP_POL_LOW, DISABLE, 0);
	}
}

int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt, uintptr_t reg)
{
#if DBG_DIRECT_SHOW
	DBG_DIRECT("[%s] pin_cnt=%d", __func__, pin_cnt);
#endif
	for (uint8_t i = 0U; i < pin_cnt; i++) {
		pinctrl_configure_pin(&pins[i]);
	}

	return 0;
}
