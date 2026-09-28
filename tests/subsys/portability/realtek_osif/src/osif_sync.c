#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <os_task.h>
#include <os_sync.h>
#include <os_sched.h>

#define WAIT_MS      50
#define TIMEOUT_MS   (100 + WAIT_MS)
#define STACKSZ         1024

ZTEST(osif_sync, test_os_lock_unlock)
{
    uint32_t key = os_lock();
    zassert_equal(__get_BASEPRI(),_EXC_IRQ_DEFAULT_PRIO,"BASEPRI is not set correctly when calling os_lock");
    os_unlock(key);
    zassert_equal(__get_BASEPRI(),0,"BASEPRI is clear correctly when calling os_unlock");
}
ZTEST_SUITE(osif_sync, NULL, NULL, NULL, NULL, NULL);
