#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <os_task.h>
#include <os_sched.h>

#include <zephyr/irq_offload.h>
#include <zephyr/kernel_structs.h>

#define STACKSZ 1024

#define TIMEOUT		(100)
#define SIGNAL1		(0x00000020)
#define SIGNAL2		(0x00000004)
#define SIGNAL		(SIGNAL1 | SIGNAL2)

#define SIGNAL_FLAG 0x00000001
#define ISR_SIGNAL 0x50

#define TIMEOUT_TICKS   (10)
#define FLAG1           (0x00000020)
#define FLAG2           (0x00000004)
#define FLAG            (FLAG1 | FLAG2)
#define ISR_FLAG        (0x50)

static void thread1(void *arg)
{
	/* wait for FLAG1. It should return immediately as it is
	 * triggered but cleared.
	 */
	bool status;
	uint32_t p_signal;
	status = os_task_signal_recv(&p_signal, TIMEOUT_TICKS);
	zassert_equal(status, false,
		      "signal wait successed unexpectedly, means signal clear is not working!");
}

static void thread2(void *arg)
{
	bool status;
	uint32_t p_signal;
	status = os_task_signal_recv(&p_signal, TIMEOUT_TICKS);
	zassert_equal(status, true,
		      "signal wait failed unexpectedly");
	zassert_equal(p_signal & FLAG, FLAG,
		      "os_task_signal_recv failed unexpectedly");

	/* validate by passing invalid parameters */
	zassert_equal(os_task_signal_send(NULL, 0), false,
		      "os_task_signal_send: Invalid Task Handle is unexpectedly working!");
}

ZTEST(osif_task_signal, test_task_signal_clear)
{
	void *id1 = NULL;
	uint32_t task_param;
	bool status;
	status = os_task_create(&id1, "task2",  thread1,
                        &task_param, STACKSZ, 6);
	zassert_true(status != false, "Failed creating thread2");

	status = os_task_signal_send(id1, FLAG1);
	zassert_true(status == true, "os_task_signal_send fail");

	status = os_task_signal_clear(id1);
	zassert_true(status == true, "os_task_signal_clear fail");

	status = os_task_signal_clear(NULL);
	zassert_true(status == false, "os_task_signal_clear: Invalid Task Handle is unexpectedly working!");

	os_delay(TIMEOUT_TICKS / 2);

	status = os_task_delete(id1);
	zassert_true(status != false, "Thread delete failed");
}


ZTEST(osif_task_signal, test_task_signalled)
{
	void *id2 = NULL;
	uint32_t task_param;
	bool status;
	status = os_task_create(&id2, "task2",  thread2,
                        &task_param, STACKSZ, 6);
	zassert_true(status != false, "Failed creating thread2");

	status = os_task_signal_send(id2, FLAG1);
	zassert_true(status == true, "os_task_signal_send fail");

	status = os_task_signal_send(id2, FLAG2);
	zassert_true(status == true, "os_task_signal_send fail");

	/* ZTEST_THREAD has a higher priority over the other threads.
	 * Hence, this thread needs to be put to sleep for thread2
	 * to become the active thread.
	 */
	os_delay(TIMEOUT_TICKS / 2);

	status = os_task_delete(id2);
	zassert_true(status != false, "Thread delete failed");
}

/* IRQ offload function handler to set signal flag */
static void offload_function(const void *param)
{
	k_tid_t tid = (k_tid_t)param;

	/* Make sure we're in IRQ context */
	zassert_true(k_is_in_isr(), "Not in IRQ context!");

	bool status = os_task_signal_send(tid, ISR_SIGNAL);
	zassert_not_equal(status, false, "signal set failed in ISR");
}

void test_signal_from_isr(void *param)
{
	void *id;
	bool status;
	uint32_t p_signal;

	status = os_task_handle_get(&id);
	/**TESTPOINT: Offload to IRQ context*/
	irq_offload(offload_function, (const void *)id);
	
	status = os_task_signal_recv(&p_signal, TIMEOUT);
	zassert_equal(status, true,
		      "signal wait failed unexpectedly");
	zassert_equal((p_signal & ISR_SIGNAL),
		       ISR_SIGNAL, "unexpected signal wait value");
}

ZTEST(osif_task_signal, test_signal_events_isr)
{
	void *id3 = NULL;
	uint32_t task_param;

	bool status;
	status = os_task_create(&id3, "test_signal_from_isr", test_signal_from_isr,
                        &task_param, STACKSZ, 6);

	zassert_true(status != false, "Thread creation failed");

	os_delay(1000);

	status = os_task_delete(id3);
	zassert_true(status != false, "Thread delete failed");
}
ZTEST_SUITE(osif_task_signal, NULL, NULL, NULL, NULL, NULL);
