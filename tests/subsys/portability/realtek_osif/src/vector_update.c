#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <soc.h>

extern IRQ_Fun *WrapperVectorTable;
extern void isr_wrapper(void);
typedef void (*zephyr_isr)(const void *);

void test_isr_xip(const void *)
{
    return;
}

__ramfunc
void test_isr_ram(const void *)
{
    return;
}

ZTEST(rtk_vector_update, test_RamVectorTableUpdate)
{
    //Test first level interrupt (xip isr)
    struct _isr_table_entry *entry = &_sw_isr_table[ASRC0_IRQn];
    zassert_equal(z_irq_spurious, entry->isr, "Wrong! The isr on zephyr sw isr table should be z_irq_spurious because it is not registered yet!");
    RamVectorTableUpdate(ASRC0_VECTORn, (IRQ_Fun)test_isr_xip);
    zassert_equal((zephyr_isr)isr_wrapper, entry->isr, "Wrong! The isr on zephyr sw isr table should be isr_wrapper(rtk vesion) because it is a xip isr!");
    zassert_equal(WrapperVectorTable[ASRC0_VECTORn], (IRQ_Fun)test_isr_xip, "Wrong! The isr on rtk wrapper table should be test_isr!");

    //Test first level interrupt (ram isr)
    RamVectorTableUpdate(ASRC0_VECTORn, (IRQ_Fun)test_isr_ram);
    zassert_equal(test_isr_ram, entry->isr, "Wrong! The isr on zephyr sw isr table should be test_isr_ram!");
}
ZTEST_SUITE(rtk_vector_update, NULL, NULL, NULL, NULL, NULL);
