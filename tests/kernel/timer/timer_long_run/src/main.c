/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/tc_util.h>
#include <zephyr/ztest.h>
#include <log_core.h>

static struct k_timer periodic_timer;
static struct k_sem periodic_sem;

/*
 * The following code collects periodic time samples using the timer's
 * auto-restart feature based on its period argument.
 */

static void timer_period_fn(struct k_timer *t)
{
	uint64_t curr_cycle;

	curr_cycle = k_cycle_get_64();

	printk("every 1 minutes, cur cycle:%lld",curr_cycle);

}

static void collect_timer_period_time_samples(void)
{
	k_timer_init(&periodic_timer, timer_period_fn, NULL);
	k_timer_start(&periodic_timer, K_NO_WAIT, K_MINUTES(1));
}


/**
 * @brief Test monotonic timer
 *
 * Validates monotonic timer's clock calibration.
 *
 * It reads the System clock’s h/w timer frequency value continuously
 * using k_cycle_get_32() to verify its working and correctness.
 * It also checks system tick frequency by checking the delta error
 * between generated and system clock provided HW cycles per sec values.
 *
 * @ingroup kernel_timer_tests
 *
 * @see k_cycle_get_32(), sys_clock_hw_cycles_per_sec()
 */
ZTEST(timer_fn, test_timer_64_cycle_long_run)
{
	k_sem_init(&periodic_sem, 0, 1);

	collect_timer_period_time_samples();

	k_sem_take(&periodic_sem, K_FOREVER);

}

ZTEST_SUITE(timer_fn, NULL, NULL, NULL, NULL, NULL);
