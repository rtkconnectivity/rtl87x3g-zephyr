#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "zephyr/kernel.h"
#include "zephyr/kernel/thread.h"
#include <zephyr/sys/sys_heap.h>
#include <zephyr/sys/multi_heap.h>
#include <zephyr/devicetree.h>
#include <soc.h>

#include "patch_os.h"
#include "mem_types.h"
#include "trace.h"
#include "osif_zephyr.h"
#include "os_mem.h"
#include "platform_utils.h"
#include "rom_symbol.h"
#include "os_queue.h"
#include "os_timer.h"

K_MEM_SLAB_DEFINE(osif_timer_slab, sizeof(struct osif_timer),
		  CONFIG_OSIF_TIMER_MAX_COUNT, __alignof__(struct osif_timer));
/************************************************************ Mem Management ************************************************************/

static struct k_heap k_heap_array[RAM_TYPE_NUM];
struct sys_multi_heap multi_heap;

#define MULTI_HEAP_ADD(ram_type, node_label) \
    k_heap_init(&k_heap_array[ram_type],    \
                (void *)(DT_REG_ADDR(DT_NODELABEL(node_label))),    \
                DT_REG_SIZE(DT_NODELABEL(node_label))); \
    sys_multi_heap_add_heap(&multi_heap, &k_heap_array[ram_type].heap, NULL);

static void *os_heap_choice(struct sys_multi_heap *mheap, void *cfg, size_t align, size_t size)
{
    void *p = NULL;
    RAM_TYPE ram_type = (RAM_TYPE)cfg;

    if (ram_type == RAM_TYPE_DATA_ON ||
        ram_type == RAM_TYPE_DATA_OFF ||
        ram_type == RAM_TYPE_DTCM ||
        ram_type == RAM_TYPE_ITCM) {
        // Try to allocate from RAM_TYPE_ITCM
        p = k_heap_aligned_alloc(&k_heap_array[RAM_TYPE_ITCM], align, size, K_NO_WAIT);
        if (p != NULL) {
            return p;
        }

        // Try to allocate from RAM_TYPE_DTCM
        p = k_heap_aligned_alloc(&k_heap_array[RAM_TYPE_DTCM], align, size, K_NO_WAIT);
        if (p != NULL) {
            return p;
        }

        // Try to allocate from RAM_TYPE_DATA_ON
        p = k_heap_aligned_alloc(&k_heap_array[RAM_TYPE_DATA_ON], align, size, K_NO_WAIT);
        if (p != NULL) {
            return p;
        }
    } else if (ram_type == RAM_TYPE_BUFFER_ON || ram_type == RAM_TYPE_BUFFER_OFF) {
        p = k_heap_aligned_alloc(&k_heap_array[RAM_TYPE_BUFFER_ON], align, size, K_NO_WAIT);
    } else {
        p = k_heap_aligned_alloc(&k_heap_array[ram_type], align, size, K_NO_WAIT);
    }

    if (p == NULL)
    {
        DBG_DIRECT("os_mem_(alligned)_alloc failed! ram_type:%d alloc_size:0x%x", ram_type, size);
#ifdef CONFIG_SYS_HEAP_RUNTIME_STATS
        // extern bool os_mem_peek_zephyr(RAM_TYPE ram_type, size_t *p_size);
        // size_t p_size;
        // os_mem_peek_zephyr(RAM_TYPE_DATA_ON, &p_size);
        // os_mem_peek_zephyr(RAM_TYPE_BUFFER_ON, &p_size);
#endif
    }

    return p;
}

void os_heap_init_zephyr(void)
{
    // init all heap region

    static atomic_t state;

    if (!atomic_cas(&state, 0, 1))
    {
        DBG_DIRECT("os_heap_ini atomic operation failed!");
        return;
    }
    sys_multi_heap_init(&multi_heap, os_heap_choice);
    atomic_set(&state, 1);

    /* ram type: RAM_TYPE_DATA_ON */
    MULTI_HEAP_ADD(RAM_TYPE_DATA_ON, heap_sram);

    /* ram type: RAM_TYPE_BUFFER_ON */
    MULTI_HEAP_ADD(RAM_TYPE_BUFFER_ON, heap_buffer_ram);

    /* ram type: RAM_TYPE_ITCM */
    MULTI_HEAP_ADD(RAM_TYPE_ITCM, heap_itcm);

    /* ram type: RAM_TYPE_DTCM */
    MULTI_HEAP_ADD(RAM_TYPE_DTCM, heap_dtcm);

    /* ram type: RAM_TYPE_DSPSHARE */
    //MULTI_HEAP_ADD(RAM_TYPE_DSPSHARE, dsp_ram_0);
}

/****************************************************************************/
/* Allocate memory                                                          */
/****************************************************************************/
bool os_mem_alloc_intern_zephyr(RAM_TYPE ram_type, size_t size, void **pp)
{
    *pp = NULL;
    *pp = sys_multi_heap_alloc(&multi_heap, (void *)ram_type, size);
    return true;
}

bool os_mem_zalloc_intern_zephyr(RAM_TYPE ram_type, size_t size, void **pp)
{
    *pp = NULL;
    *pp = sys_multi_heap_alloc(&multi_heap, (void *)ram_type, size);
    if (*pp != NULL) {
        memset(*pp, 0, size);
    }
    return true;
}

/****************************************************************************/
/* Allocate aligned memory                                                  */
/****************************************************************************/
bool os_mem_aligned_alloc_intern_zephyr(RAM_TYPE ram_type, size_t size, uint8_t alignment,
                                         void **pp)
{
    *pp = NULL;
    *pp = sys_multi_heap_aligned_alloc(&multi_heap, (void *)ram_type,
                                       alignment, size);
    return true;
}

/****************************************************************************/
/* Free memory                                                              */
/****************************************************************************/
bool os_mem_free_zephyr(void *p_block)
{
    if(p_block == NULL) {
        DBG_DIRECT("%s: cannot free a null pointer!",__func__);
        return true;
    }
    sys_multi_heap_free(&multi_heap, p_block);
    return true;
}

/****************************************************************************/
/* Free aligned memory                                                      */
/****************************************************************************/
bool os_mem_aligned_free_zephyr(void *p_block)
{
    if(p_block == NULL) {
        DBG_DIRECT("%s: cannot free a null pointer!",__func__);
        return true;
    }
    sys_multi_heap_free(&multi_heap, p_block);
    return true;
}

/****************************************************************************/
/* Peek unused (available) memory size                                      */
/****************************************************************************/
bool os_mem_peek_zephyr(RAM_TYPE ram_type, size_t *p_size)
{
#ifdef CONFIG_SYS_HEAP_RUNTIME_STATS
    struct sys_memory_stats stats;
    uint32_t heap_size;

    switch (ram_type) {
    case RAM_TYPE_DATA_ON:
        heap_size = DT_REG_SIZE(DT_NODELABEL(heap_sram));
        break;
    case RAM_TYPE_BUFFER_ON:
        heap_size = DT_REG_SIZE(DT_NODELABEL(heap_buffer_ram));
        break;
    case RAM_TYPE_ITCM:
        heap_size = DT_REG_SIZE(DT_NODELABEL(heap_itcm));
        break;
    case RAM_TYPE_DTCM:
        heap_size = DT_REG_SIZE(DT_NODELABEL(heap_dtcm));
        break;
    default:
        printk("Invalid ram_type %d for os_mem_peek!\n",ram_type);
        return true;
    }

    // printk("RAM type of heap: %d, heap size: %zu, allocated %zu, free %zu, max allocated %zu\n",
    //        ram_type, heap_size, stats.allocated_bytes, stats.free_bytes,
    //        stats.max_allocated_bytes);

    sys_heap_runtime_stats_get(&(k_heap_array[ram_type].heap), &stats);
    *p_size = stats.free_bytes;

    /* use DBG_DIRECT when zephyr log system is not initialized*/
    DBG_DIRECT("RAM type of heap: %d, heap size: %d, allocated %d, free %d, max allocated %d\n",
               ram_type, heap_size, stats.allocated_bytes, stats.free_bytes,
               stats.max_allocated_bytes);

#else
    DBG_DIRECT("System heap runtime statistics not enabled");
#endif
    return true;
}

/* No patch pointer, so just realize it. */
void os_mem_peek_printf(void)
{
#ifdef CONFIG_SYS_HEAP_RUNTIME_STATS
    DBG_DIRECT("heap memory scan:sram %d, buffer ram %d, DTCM %d, ITCM %d",
                    os_mem_peek(RAM_TYPE_DATA_ON),
                    os_mem_peek(RAM_TYPE_BUFFER_ON), os_mem_peek(RAM_TYPE_DTCM),
                    os_mem_peek(RAM_TYPE_ITCM));
#else
    DBG_DIRECT("System heap runtime statistics not enabled");
#endif
    return;
}

/************************************************************ msg queue ************************************************************/
bool os_msg_queue_create_intern_zephyr(void **pp_handle, const char *p_name, uint32_t msg_num, uint32_t msg_size,
                                        bool *p_result)
{
    int ret;
    struct k_msgq *queue_obj;
    void *queue_buffer;
    size_t total_size = msg_size * msg_num;
    if (pp_handle)
    {
        /* alloc k_msgq */
        queue_obj = sys_multi_heap_alloc(&multi_heap, (void *)RAM_TYPE_DATA_ON, sizeof(struct k_msgq));
        if (queue_obj != NULL)
        {
            memset(queue_obj, 0, sizeof(struct k_msgq));
            *pp_handle = queue_obj;

            /* alloc msgq buffer */
            queue_buffer = sys_multi_heap_aligned_alloc(&multi_heap, (void *)RAM_TYPE_DATA_ON,
                                                        4, total_size);
            if (queue_buffer != NULL)
            {
                k_msgq_init(queue_obj, queue_buffer, msg_size, msg_num);
                queue_obj->flags = K_MSGQ_FLAG_ALLOC;
                ret = 0;
            }
            else
            {
                sys_multi_heap_free(&multi_heap, queue_obj);
                DBG_DIRECT("alloc queue buffer failed because data ram heap is full");
                ret = -ENOMEM;
            }
        }
        else
        {
            DBG_DIRECT("alloc queue object failed because data ram heap is full");
            ret = -ENOMEM;
        }

        *p_result = (ret == 0) ? true : false;

    }
    else
    {
        *p_result = false;
    }

    return true;
}

bool os_msg_queue_delete_intern_zephyr(void *p_handle, bool *p_result)
{
    struct k_msgq *obj;

    obj = (struct k_msgq *)p_handle;

    if ((obj->flags & K_MSGQ_FLAG_ALLOC) != 0U)
    {
        sys_multi_heap_free(&multi_heap, obj->buffer_start);
        obj->buffer_start = NULL;
        obj->flags &= ~K_MSGQ_FLAG_ALLOC;
        sys_multi_heap_free(&multi_heap, obj);
        *p_result = true;
        return true;
    }
    *p_result = false;
    return true;
}

bool os_msg_queue_peek_intern_zephyr(void *p_handle, uint32_t *p_msg_num, bool *p_result)
{
    struct k_msgq *obj;

    if (p_handle)
    {
        obj = (struct k_msgq *)p_handle;
        *p_msg_num = k_msgq_num_used_get(obj);
        *p_result = true;
    }
    else
    {
        *p_result = false;
    }
    return true;

}

bool os_msg_send_intern_zephyr(void *p_handle, void *p_msg, uint32_t wait_ms, bool *p_result)
{
    int ret;
    struct k_msgq *obj;
    k_timeout_t wait_ticks;

    obj = (struct k_msgq *)p_handle;

    wait_ticks = (wait_ms == 0xFFFFFFFFUL) ? K_FOREVER : K_MSEC(wait_ms);
    ret = k_msgq_put(obj, p_msg, wait_ticks);

    *p_result = (ret == 0) ? true : false;
    return true;
}

bool os_msg_recv_intern_zephyr(void *p_handle, void *p_msg, uint32_t wait_ms, bool *p_result)
{
    int ret;
    struct k_msgq *obj;
    k_timeout_t wait_ticks;

    obj = (struct k_msgq *)p_handle;

    wait_ticks = (wait_ms == 0xFFFFFFFFUL) ? K_FOREVER : K_MSEC(wait_ms);
    ret = k_msgq_get(obj, p_msg, wait_ticks);

    *p_result = (ret == 0) ? true : false;
    return true;
}
// /************************************************************ SCHEDULAR ************************************************************/

bool os_sched_start_zephyr(bool *p_result)
{
    //zephyr not supported.
    DBG_DIRECT("zephyr does not support os_sched_start!");
    *p_result = false;
    return true;
}

bool os_sched_stop_zephyr(bool *p_result)
{
    //zephyr not supported.
    DBG_DIRECT("zephyr does not support os_sched_stop!");
    *p_result = false;
    return true;
}

bool os_sched_suspend_zephyr(bool *p_result)
{
    *p_result = true;
    k_sched_lock();
    return true;
}

bool os_sched_resume_zephyr(bool *p_result)
{
    *p_result = true;
    k_sched_unlock();
    return true;
}
extern struct k_thread *z_swap_next_thread(void);

bool os_sched_state_get_zephyr(long *p_state, bool *p_result)
{

    if (z_swap_next_thread() == NULL)
    {
        *p_state = SCHEDULER_NOT_STARTED;
    }
    else if (_current->base.sched_locked != 0U)
    {
        *p_state = SCHEDULER_SUSPENDED;
    }
    else
    {
        *p_state = SCHEDULER_RUNNING;
    }

    *p_result = true;
    return true;
}

bool os_delay_zephyr(uint32_t ms)
{
    k_sleep(K_MSEC(ms));
    return true;
}

// /************************************************************ SYSTICK ************************************************************/
// bool os_systick_handler_zephyr(void)
// {
//     // ToDo
//     // extern void sys_clock_isr(void *arg);
//     // sys_clock_isr(NULL);
//     sys_clock_announce_only_add_ticks(1);
//     return true;
// }

bool os_sys_time_get_zephyr(uint64_t *p_time_ms)
{
    *p_time_ms = (uint64_t)k_uptime_get();
    return true;
}

// bool os_sys_tick_get_zephyr(uint64_t *p_sys_tick)
// {
//     *p_sys_tick = (uint64_t)k_uptime_ticks();

//     return true;
// }

// bool os_sys_tick_increase_zephyr(uint32_t tick_increment,
//                                  uint64_t *p_old_tick)
// {
//     *p_old_tick = sys_clock_tick_get();
//     sys_clock_announce_only_add_ticks(tick_increment);
//     return true;
// }
// /************************************************************ sync ************************************************************/

bool os_lock_zephyr(uint32_t *p_flags)
{
    *p_flags = arch_irq_lock();
    return true;
}

bool os_unlock_zephyr(uint32_t flags)
{
    arch_irq_unlock(flags);
    return true;
}

bool os_sem_create_zephyr(void **pp_handle, const char *p_name, uint32_t init_count,
                          uint32_t max_count, bool *p_result)
{
    struct k_sem *sem_obj;
    sem_obj = (struct k_sem *) sys_multi_heap_alloc(&multi_heap, RAM_TYPE_DATA_ON,
                                                    sizeof(struct k_sem));

    if (sem_obj != NULL)
    {
        memset(sem_obj, 0, sizeof(struct k_sem));
    }
    else
    {
        DBG_DIRECT("alloc sem object failed because data ram heap is full");
        *p_result = false;
        return true;
    }

    k_sem_init(sem_obj, init_count, max_count);
    *pp_handle = sem_obj;
    *p_result = true;
    return true;
}

bool os_sem_delete_zephyr(void *p_handle, bool *p_result)
{
    struct k_sem *obj;

    if (p_handle == NULL)
    {
        DBG_DIRECT("%s: Sem to delete is a NULL pointer!",__func__);
        *p_result = false;
        return true;
    }

    obj = (struct k_sem *)p_handle;

    sys_multi_heap_free(&multi_heap, obj);
    *p_result = true;
    return true;
}

bool os_sem_take_zephyr(void *p_handle, uint32_t wait_ms, bool *p_result)
{
    int ret;

    struct k_sem *obj;
    k_timeout_t wait_ticks;

    if (p_handle == NULL)
    {
        DBG_DIRECT("%s: Sem pointer is NULL, take fail!",__func__);
        *p_result = false;
        return true;
    }

    obj = (struct k_sem *)p_handle;

    wait_ticks = (wait_ms == 0xFFFFFFFFUL) ? K_FOREVER : K_MSEC(wait_ms);
    ret = k_sem_take(obj, wait_ticks);

    *p_result = (ret == 0) ? true : false;
    return true;
}

bool os_sem_give_zephyr(void *p_handle, bool *p_result)
{
    struct k_sem *obj;

    if (p_handle == NULL)
    {
        DBG_DIRECT("%s: Sem pointer is NULL, give fail!",__func__);
        *p_result  = false;
        return true;
    }
    else
    {
        obj = (struct k_sem *)p_handle;
        /* All tokens have already been released */
        if (k_sem_count_get(obj) == obj->limit) {
            DBG_DIRECT("%s: All tokens have already been released", __func__);
            *p_result  = false;
            return true;
        }

        k_sem_give(obj);

        *p_result  = true;
        return true;
    }

}

bool os_mutex_create_zephyr(void **pp_handle, bool *p_result)
{
    struct k_mutex *mutex_obj;
    mutex_obj = (struct k_mutex *) sys_multi_heap_alloc(&multi_heap, RAM_TYPE_DATA_ON,
                                                        sizeof(struct k_mutex));

    if (mutex_obj != NULL)
    {
        memset(mutex_obj, 0, sizeof(struct k_mutex));
        k_mutex_init(mutex_obj);
        *pp_handle = mutex_obj;
        *p_result  = true;
        return true;
    }
    else
    {
        DBG_DIRECT("alloc mutex object failed because data ram heap is full");
        *p_result  = false;
        return true;
    }
}

bool os_mutex_delete_zephyr(void *p_handle, bool *p_result)
{
    struct k_mutex *obj;

    if (p_handle == NULL)
    {
        DBG_DIRECT("%s: Mutex pointer is NULL, delete fail!",__func__);
        *p_result  = false;
        return true;
    }

    obj = (struct k_mutex *)p_handle;

    sys_multi_heap_free(&multi_heap, obj);
    *p_result  = true;
    return true;
}

bool os_mutex_take_zephyr(void *p_handle, uint32_t wait_ms, bool *p_result)
{
    int ret;

    struct k_mutex *obj;
    k_timeout_t wait_ticks;

    if (p_handle == NULL)
    {
        DBG_DIRECT("Mutex pointer is NULL, take fail!");
        *p_result  = false;
        return true;
    }

    obj = (struct k_mutex *)p_handle;

    wait_ticks = (wait_ms == 0xFFFFFFFFUL) ? K_FOREVER : K_MSEC(wait_ms);
    ret = k_mutex_lock(obj, wait_ticks);

    *p_result = (ret == 0) ? true : false;
    return true;
}

bool os_mutex_give_zephyr(void *p_handle, bool *p_result)
{
    int ret;

    struct k_mutex *obj;

    if (p_handle == NULL)
    {
        DBG_DIRECT("Mutex pointer is NULL, give fail!");
        *p_result  = false;
        return true;
    }

    obj = (struct k_mutex *)p_handle;

    ret = k_mutex_unlock(obj);

    *p_result = (ret == 0) ? true : false;
    return true;

}


// /* ************************************************* OSIF TASK ************************************************* */
K_MEM_SLAB_DEFINE(osif_task_slab, sizeof(struct osif_task),
		  CONFIG_OSIF_TASK_MAX_COUNT, 4);

bool os_task_create_zephyr(void **pp_handle, const char *p_name, void (*p_routine)(void *),
                           void *p_param, uint16_t stack_size, uint16_t priority, bool *p_is_create_success)
{
    struct osif_task *task;

	if (k_mem_slab_alloc(&osif_task_slab, (void **)&task, K_NO_WAIT) == 0) {
		(void)memset(task, 0, sizeof(struct osif_task));
	} else {
        DBG_DIRECT("Exceed Max number of tasks: %d!",CONFIG_OSIF_TASK_MAX_COUNT);
        *p_is_create_success = false;
		return true;
	}

    k_tid_t thread_id;

    int switch_priority = CONFIG_ZEPHYR_PRI_MAX - priority;

    /****************************** dynamic stack ******************************/
    k_thread_stack_t *stack_buffer;
    stack_buffer = (k_thread_stack_t *) sys_multi_heap_aligned_alloc(&multi_heap, RAM_TYPE_DATA_ON,
                                                                     8, stack_size);
    if (stack_buffer == NULL)
    {
        k_mem_slab_free(&osif_task_slab, (void *)task);
        task = NULL;
        DBG_DIRECT("alloc thread stack failed because data ram heap is full");
        *p_is_create_success = false;
        return true;
    }

    memset(stack_buffer, 0, stack_size);
    task->stack_start = stack_buffer;

    k_poll_signal_init(&task->poll_signal);
	k_poll_event_init(&task->poll_event, K_POLL_TYPE_SIGNAL,
			K_POLL_MODE_NOTIFY_ONLY, &task->poll_signal);
    task->signal_results = 0U;

    *pp_handle = task;//where should it be placed? before k_thread_create or after k_thread_create.
    thread_id = k_thread_create(&task->zthread, stack_buffer, stack_size,
                                (k_thread_entry_t) p_routine, p_param, NULL, NULL,
                                switch_priority, 0, K_NO_WAIT);
    
    if (thread_id != NULL)
    {
        k_thread_name_set(thread_id, p_name);
    }

    *p_is_create_success = true;
    return true;
}

bool os_task_delete_zephyr(void *p_handle, bool *p_result)
{
    struct osif_task *tid = (struct osif_task *)p_handle;

    if(tid == NULL) {
        DBG_DIRECT("os_task_delete error!");
        *p_result = false;
        return true;
    }

    k_thread_abort(&tid->zthread);

    sys_multi_heap_free(&multi_heap, tid->stack_start);

    k_mem_slab_free(&osif_task_slab, (void *)tid);

    *p_result = true;
    return true;
}

bool os_task_suspend_zephyr(void *p_handle, bool *p_result)
{
    struct osif_task *tid = (struct osif_task *)p_handle;

    if(tid == NULL) {
        DBG_DIRECT("os_task_suspend error!");
        *p_result = false;
        return true;
    }

    k_thread_suspend(&tid->zthread);

    *p_result = true;
    return true;
}

bool os_task_resume_zephyr(void *p_handle, bool *p_result)
{
    struct osif_task *tid = (struct osif_task *)p_handle;

    if(tid == NULL) {
        DBG_DIRECT("os_task_resume error!");
        *p_result = false;
        return true;
    }

    k_thread_resume(&tid->zthread);

    *p_result = true;
    return true;
}

bool os_task_yield_zephyr(bool *p_result)
{
    if (k_is_in_isr()) {
        DBG_DIRECT("cannot yield task in isr!");
        *p_result = false;
		return true;
	}

    k_yield();

    *p_result = true;
    return true;
}

bool os_task_handle_get_zephyr(void **pp_handle, bool *p_result)
{
    if (pp_handle == NULL)
    { 
        *p_result = false;
        return true;
    }

    struct osif_task *tid;
    k_tid_t thread;
    thread = k_current_get();
    tid = CONTAINER_OF(thread, struct osif_task, zthread);

    *pp_handle = tid;
    *p_result = true;
    return true;
}

bool os_task_priority_get_zephyr(void *p_handle, uint16_t *p_priority, bool *p_result)
{
    if (p_priority == NULL)
    {
        DBG_DIRECT("%s, p_priority is NULL",__func__);
        *p_result = false;
        return true;
    }

    struct osif_task *tid = (struct osif_task *)p_handle;

    if(tid == NULL) {
        DBG_DIRECT("os_task_priority_get error!");
        *p_result = false;
        return true;
    }

    k_tid_t thread;
    uint16_t priority;

    if (tid != NULL)
    {
        thread = &tid->zthread;
    }
    else
    {
        thread = k_current_get();
    }

    priority = k_thread_priority_get(thread);
    *p_priority = CONFIG_ZEPHYR_PRI_MAX - priority;

    *p_result = true;
    return true;
}

bool os_task_priority_set_zephyr(void *p_handle, uint16_t priority, bool *p_result)
{
    if (priority > 6 || priority < 0) {
        *p_result = false;
        DBG_DIRECT("%s: Invalid priority. Priority is expected to 0 ~ 6.",__func__);
        return true;
    }

    struct osif_task *tid = (struct osif_task *)p_handle;

    uint16_t switch_priority = CONFIG_ZEPHYR_PRI_MAX - priority;

    k_tid_t thread;

    if (tid != NULL)
    {
        thread = &tid->zthread;
    }
    else
    {
        thread = k_current_get();
    }

    k_thread_priority_set(thread, switch_priority);

    *p_result = true;
    return true;
}

bool os_task_signal_create_zephyr(void *p_handle, uint32_t count, bool *p_result)
{
    ARG_UNUSED(count);
    ARG_UNUSED(p_handle);
    ARG_UNUSED(p_result);
    return true;
}

bool os_task_signal_send_zephyr(void *p_handle, uint32_t signal, bool *p_result)
{
    int key;
    
    struct osif_task *tid = (struct osif_task *)p_handle;

	if ((tid == NULL) || (!signal)) {
        *p_result = false;
        DBG_DIRECT("%s: Target task handle is a NULL pointer, or input signal is 0!", __func__);
		return true;
	}

    key = irq_lock();
    /* os_task_signal_send_freertos call 
     * xTaskNotify((TaskHandle_t)p_handle, signal, eSetBits),
     * in which eSetBits means doing a or operation.  */
	tid->signal_results |= signal;
	irq_unlock(key);

    k_poll_signal_raise(&tid->poll_signal, signal);

    *p_result = true;
    return true;
}

bool os_task_signal_recv_zephyr(uint32_t *p_signal, uint32_t wait_ms, bool *p_result)
{
// wait any signal bit!!!
    int retval, key;

    if (p_signal == NULL) {
        *p_result = false;
        DBG_DIRECT("%s: input signal pointer cannot be NULL!",__func__);
        return true;
    }

	if (k_is_in_isr()) {
		*p_result = false;
        DBG_DIRECT("%s cannot be called in ISR!",__func__);
		return true;
	}

    k_tid_t thread = k_current_get();
    struct osif_task *tid =  CONTAINER_OF(thread, struct osif_task, zthread);

    switch (wait_ms) {
    case 0:
        retval = k_poll(&tid->poll_event, 1, K_NO_WAIT);
        break;
    case 0xFFFFFFFFU:
        retval = k_poll(&tid->poll_event, 1, K_FOREVER);
        break;
    default:
        retval = k_poll(&tid->poll_event, 1,
                K_MSEC(wait_ms));
        break;
    }

    switch (retval) {
    case 0:
        break;
    case -EAGAIN:
        *p_result = false;
        DBG_DIRECT("%s timeout!",__func__);
        return true;
    default:
        *p_result = false;
        DBG_DIRECT("%s error ret value, %d!",__func__, retval);
        return true;
    }

    __ASSERT(tid->poll_event.state
            == K_POLL_STATE_SIGNALED,
        "event state not signalled!");
    __ASSERT(tid->poll_event.signal->signaled == 1,
        "event signaled is not 1");

    key = irq_lock();
    /* Reset the states to facilitate the next trigger */
    tid->poll_event.signal->signaled = 0;
    tid->poll_event.state = K_POLL_STATE_NOT_READY;
	*p_signal = tid->signal_results;
	/* Clear signal flags as the thread is ready now */ 
	tid->signal_results = 0;
	irq_unlock(key);

    *p_result = true;

	return true;
}

bool os_task_signal_clear_zephyr(void *p_handle, bool *p_result)
{
    int key;

    struct osif_task *tid = (struct osif_task *)p_handle;

    if (tid == NULL) {
        DBG_DIRECT("%s: osif-task has not created!",__func__);
        *p_result = false;
        return true;
    }

    key = irq_lock();
    /* Reset the states to facilitate the next trigger */
	tid->poll_event.signal->signaled = 0;
	tid->poll_event.state = K_POLL_STATE_NOT_READY;

    /* Freertos only call xTaskNotifyStateClear, does not clear the signal value. So comment it.*/ 
	/* ts->signal_results = 0; */

    irq_unlock(key);

    *p_result = true;
    
    return true;
}


// bool os_task_status_dump_zephyr(void)
// {
//     struct k_thread *current_thread  = k_current_get();
//     uint16_t priority = k_thread_priority_get(current_thread);
//     /* To check */
//     DBG_DIRECT("pxCurrentTCB %p, pxTopOfStack %p,taks_pri:%d",
//                current_thread, current_thread ->callee_saved.psp,
//                CONFIG_ZEPHYR_PRI_MAX - priority);
//     return true;
// }

// /* ************************************************* OSIF TIMER ************************************************* */

// static struct osif_timer osif_timer_pool[CONFIG_OSIF_TIMER_MAX_COUNT];

bool os_timer_create_zephyr(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                            uint32_t interval_ms, bool reload, void (*p_timer_callback)(), bool *p_result)
{
    struct osif_timer *timer;

    if (reload != TimerOnce && reload != TimerPeriodic) {
		return NULL;
	}

	if (k_mem_slab_alloc(&osif_timer_slab, (void **)&timer, K_NO_WAIT) == 0) {
		(void)memset(timer, 0, sizeof(struct osif_timer));
	} else {
        DBG_DIRECT("Exceed Max number of timers: %d!",CONFIG_OSIF_TIMER_MAX_COUNT);
		return NULL;
	}

    timer->name = p_timer_name;
    timer->timer_id = timer_id;
    timer->interval_ms = interval_ms;
    timer->status = NOT_ACTIVE;
    timer->type = reload;

    k_timer_init(&timer->ztimer, (k_timer_expiry_t) p_timer_callback, NULL);

    uint32_t key = irq_lock();
    *pp_handle = timer;
    irq_unlock(key);

    *p_result = true;
    return true;

}

bool os_timer_start_zephyr(void **pp_handle, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;
    
    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL!",__func__);
        *p_result = false;
        return true;
    }

    if (timer->type == TimerPeriodic)
    {
        k_timer_start(&timer->ztimer, K_MSEC(timer->interval_ms), K_MSEC(timer->interval_ms));
    }
    else if (timer->type == TimerOnce)
    {
        k_timer_start(&timer->ztimer, K_MSEC(timer->interval_ms), K_NO_WAIT);
    }

    timer->status = ACTIVE;

    *p_result = true;
    return true;
}

bool os_timer_restart_zephyr(void **pp_handle, uint32_t interval_ms, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;
    
    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL!",__func__);
        *p_result = false;
        return true;
    }

    uint32_t key = irq_lock();
    timer->interval_ms = interval_ms;
    irq_unlock(key);

    if (timer->type == TimerPeriodic)
    {
        k_timer_start(&timer->ztimer, K_MSEC(timer->interval_ms), K_MSEC(timer->interval_ms));
    }
    else if (timer->type == TimerOnce)
    {
        k_timer_start(&timer->ztimer, K_MSEC(timer->interval_ms), K_NO_WAIT);
    }

    timer->status = ACTIVE;

    *p_result = true;
    return true;
}

bool os_timer_stop_zephyr(void **pp_handle, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;

    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL!",__func__);
        *p_result = false;
        return true;
    }

    if (timer->status == NOT_ACTIVE) {
		DBG_DIRECT("%s: timer is inactive, cannot stop!",__func__);
        *p_result = false;
        return true;
	}

    k_timer_stop(&timer->ztimer);

    *p_result = true;
    return true;
}

bool os_timer_delete_zephyr(void **pp_handle, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;

    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL!",__func__);
        *p_result = false;
        return true;
    }
    
    if (timer->status == ACTIVE) {
		k_timer_stop(&timer->ztimer);
		timer->status = NOT_ACTIVE;
	}

    k_mem_slab_free(&osif_timer_slab, (void *)timer);

    uint32_t key = irq_lock();
    *pp_handle = NULL;
    irq_unlock(key);

    *p_result = true;
    return true;
}

bool os_timer_handle_get_zephyr(void **pp_handle, uint8_t timer_id, bool *p_result)
{
    DBG_DIRECT("os_timer_handle_get not supported in zephyr!");
    *p_result = false;
    return true;
}

bool os_timer_id_get_zephyr(void **pp_handle, uint32_t *p_timer_id, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: Variable timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;

    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL, or not declare a timer handle pointer!",__func__);
        *p_result = false;
        return true;
    }
    *p_timer_id = timer->timer_id;
    *p_result = true;
    return true;
}

bool os_timer_state_get_zephyr(void **pp_handle, uint8_t *p_timer_state, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;

    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL!",__func__);
        *p_result = false;
        return true;
    }

    uint32_t key = irq_lock();
    if (!(k_timer_remaining_get(&timer->ztimer))) {
        *p_timer_state = 0; // timer has expired or stopped.
    } else {
        *p_timer_state = 1; // timer is running.
    }
    irq_unlock(key);

    *p_result = true;
    return true;
}

bool os_timer_auto_reload_get_zephyr(void **pp_handle, long *p_autoreload, bool *p_result)
{
    if (pp_handle == NULL) {
        DBG_DIRECT("%s: timer handle pointer is not declared!",__func__);
        *p_result = false;
        return true;
    }

    struct osif_timer *timer = (struct osif_timer *)*pp_handle;

    if (timer == NULL) {
        DBG_DIRECT("%s: timer handle pointer is NULL!",__func__);
        *p_result = false;
        return true;
    }

    *p_autoreload = timer->type;
    *p_result = true;
    return true;
}
/* ************************************************* RamVectorTableUpdate ************************************************* */
#define VECTORn_TO_IRQn(v_num)   ((int32_t)(v_num) - 16)
extern IRQ_Fun *WrapperVectorTable;
extern bool RamVectorTableUpdate_rom(uint32_t v_num, IRQ_Fun isr_handler);
extern void isr_wrapper(void);

bool rtk_update_xip_isr(uint32_t v_num, IRQ_Fun isr_handler)
{
    if (WrapperVectorTable == NULL)
    {
        WrapperVectorTable = os_mem_zalloc(RAM_TYPE_DATA_ON, MAX_IRQn * sizeof(IRQ_Fun));
        if (WrapperVectorTable == NULL)
        {
            return false;
        }
    }
    int32_t irqn = VECTORn_TO_IRQn(v_num);
    if (irqn < MAX_IRQn)
    {
        // first level interrupt
        if (NVIC_GetEnableIRQ(irqn) == 1) {
            NVIC_DisableIRQ(irqn);
            z_isr_install(irqn, (void *)isr_wrapper, NULL);
            NVIC_EnableIRQ(irqn);
        } else {
            z_isr_install(irqn, (void *)isr_wrapper, NULL);
        }
        WrapperVectorTable[v_num] = isr_handler;
    }
    else
    {
        // second level interrupt
        uint8_t first_irqn = GET_FIRST_IRQn(irqn);

        //check on zephyr sw isr table, the first level vector is  rtk_isr_wrapper or not. 
        struct _isr_table_entry *entry = &_sw_isr_table[first_irqn];
        if ((IRQ_Fun)entry->isr != isr_wrapper)
        {
            WrapperVectorTable[IRQn_TO_VECTORn(first_irqn)] = (IRQ_Fun) entry->isr;
            if (NVIC_GetEnableIRQ(first_irqn) == 1) {
                NVIC_DisableIRQ(first_irqn);
                z_isr_install(first_irqn, (void *)isr_wrapper, NULL);
                NVIC_EnableIRQ(first_irqn);
            } else {
                z_isr_install(first_irqn, (void *)isr_wrapper, NULL);
            }
        }

        RamVectorTableUpdate_rom(v_num, isr_handler);
    }
    return true;
}

bool rtk_update_ram_isr(uint32_t v_num, IRQ_Fun isr_handler)
{
    int32_t irqn = VECTORn_TO_IRQn(v_num);
    if (irqn < MAX_IRQn)
    {
        //first level interrupt
        if (NVIC_GetEnableIRQ(irqn) == 1) {
            NVIC_DisableIRQ(irqn);
            z_isr_install(irqn, (void *)isr_handler, NULL);
            NVIC_EnableIRQ(irqn);
        } else {
            z_isr_install(irqn, (void *)isr_handler, NULL);
        }
        return true;
    }
    else
    {
        //second level interrupt use rtk realization.
        return RamVectorTableUpdate_rom(v_num, isr_handler); 
    }
}

bool RamVectorTableUpdate_zephyr(uint32_t v_num, IRQ_Fun isr_handler, bool *ret){
    
    //exception or RAM isr, directly replace the vector.//in zephyr, directly register ISR is sw isr table.
    if (v_num == NMI_VECTORn) {
        z_arm_nmi_set_handler(isr_handler);
        *ret = true;
        return true;
    }

    if (v_num < System_VECTORn) {
        DBG_DIRECT("Warning! Updating exceptions' ISR! Vector number: %d",v_num);
        RamVectorTableUpdate_rom(v_num, isr_handler);
        *ret = true;
        return true;
    }

    if (SCB->VTOR == 0) {
        DBG_DIRECT("Can not update vector table in ROM!");
        *ret = false;
        return true;
    }
    if (isr_handler == NULL) {
        DBG_DIRECT("ISR_Handler to update is NULL!");
        *ret = false;
        return true;
    }

    if(check_code_is_xip((uint32_t)isr_handler)) { 
        *ret = rtk_update_xip_isr(v_num, isr_handler);
    } else {
        *ret = rtk_update_ram_isr(v_num, isr_handler); 
    }
    return true;
}

/* ************************************************* OSIF Power Mananger ************************************************* */
#include <os_pm.h>
#include <kernel_internal.h> //for z_idle_threads and arch_kernel_init
extern OSPMSystem os_pm_system;

extern struct _timeout *get_first_timeout(void);
extern struct _timeout *get_next_timeout(struct _timeout *);
extern void z_timer_expiration_handler(struct _timeout *t);
extern void z_thread_timeout(struct _timeout *t);
extern void sys_clock_announce_only_add_ticks(int32_t ticks);

extern int32_t z_get_next_timeout_expiry(void);

bool os_pm_store_zephyr(void)
{
    //------should not change order-------//
    os_pm_system.last_sleep_clk = platform_rtc_get_counter();

    /* stop systick */
    SysTick->CTRL = 0;

    /* update xTickCount and store systick current value */
    os_pm_system.last_sleep_systick = os_pm_get_valid_systick_current_value(true);

    return true;
}

bool os_pm_restore_zephyr(void)
{
    /* restore systick information then resume all task */
    os_pm_restore_os_tick_count();

    return true;
}

void pendcall_handler(struct k_work *item)
{
    struct pend_call *pc = CONTAINER_OF(item, struct pend_call, work);
    pc->pend_func(pc->para1, pc->para2);
    sys_multi_heap_free(&multi_heap, pc);
}

bool os_timer_pend_function_call_zephyr(void (*p_pend_function)(void *, uint32_t),
                                 void *para1, uint32_t para2, bool *ret)
{
    int err;
    struct pend_call *pc = sys_multi_heap_alloc(&multi_heap, (void *)RAM_TYPE_DATA_ON, sizeof(struct pend_call));

    if (pc != NULL)
    {
        pc->pend_func = p_pend_function;
        pc->para1 = para1;
        pc->para2 = para2;
        k_work_init(&pc->work, pendcall_handler);
        err = k_work_submit(&pc->work);
    }
    else
    {
        DBG_DIRECT("%s: Alloc pendcall data failed because data ram heap is full",__func__);//pend stage cannot use DBG_DIRECT? other wise it would trigger hf.
        err = -10;
    }

    *ret = (err >= 0) ? true : false;

    return true;
}

bool os_systick_handler_zephyr(void)
{
    sys_clock_announce_only_add_ticks(1);
    return true;
}

 bool os_sys_tick_increase_zephyr(uint32_t tick_increment, uint64_t *ret)
 {
    int64_t old_tick = sys_clock_tick_get();
    sys_clock_announce_only_add_ticks(tick_increment);
    *ret = (uint64_t)old_tick;
    return true;
 }

bool os_sys_tick_rate_get_zephyr(uint32_t *ret)
{
    *ret = (uint32_t)CONFIG_SYS_CLOCK_TICKS_PER_SEC;
    return true;
}

bool os_sys_tick_clk_get_zephyr(uint32_t *ret)
{
    *ret = (uint32_t)CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
    return true;
}

extern T_OS_QUEUE lpm_excluded_handle[PLATFORM_PM_EXCLUDED_TYPE_MAX];
bool os_pm_find_nearest_timeout_tick_zephyr(uint32_t *ret)
{
    //*ret = (uint32_t)z_get_next_timeout_expiry();

    /* Solution with the exclude timer mechanism. */
    uint32_t timeout_tick_res = 0xFFFFFFFF;
    uint32_t timeout_tick = 0;
    void *timer;
    void *thread;

    bool handle_checked;

    for (struct _timeout *t = get_first_timeout(); t != NULL; t = get_next_timeout(t))
    {
        handle_checked = true;
        timeout_tick += t->dticks;

        if (t->fn == z_timer_expiration_handler) //z_timer_expiration_handler
        {
            /* (struct k_timer *) ==  (struct osif_timer *) because struct k_timer is the first element of struct osif_timer
            so no matter we register osif_timer or k_timer, the pointer would be the same.*/
            timer = (void *) CONTAINER_OF(t, struct k_timer, timeout); 

            T_OS_QUEUE_ELEM *p_cur_queue_item = lpm_excluded_handle[0].p_first;
            while (p_cur_queue_item != NULL)
            {
                void *cur_excluded_handle = *(((PlatformPMExcludedHandleQueueElem *)p_cur_queue_item)->handle);
                if (cur_excluded_handle != NULL)
                {
                    if (timer == cur_excluded_handle)
                    {
                        long is_auto_reload;
                        os_timer_auto_reload_get(&cur_excluded_handle, &is_auto_reload);
                        if (is_auto_reload)
                        {
                            DBG_DIRECT("[PM]!!handle=0x%x, exclude timer cannot be auto_reload\n",
                                    (unsigned int)cur_excluded_handle);
                            __ASSERT(0, "[PM]!!handle=0x%x", (unsigned int) cur_excluded_handle);
                        }
                        handle_checked = false;
                        break;
                    }
                }
                p_cur_queue_item = p_cur_queue_item->p_next;
            }

            if (handle_checked)
            {
                timeout_tick_res = timeout_tick;
                break;
            }
        }
        else if (t->fn == z_thread_timeout) //z_thread_timeout
        {
            /* (struct k_thread *) ==  (struct osif_task *) because struct k_thread is the first element of struct osif_task.
            so no matter we register osif_task or k_thread, the pointer would be the same.*/
            thread = (void *)CONTAINER_OF(t, struct k_thread, base.timeout);

            T_OS_QUEUE_ELEM *p_cur_queue_item = lpm_excluded_handle[1].p_first;
            while (p_cur_queue_item != NULL)
            {
                void *cur_excluded_handle = *(((PlatformPMExcludedHandleQueueElem *)p_cur_queue_item)->handle);
                if (cur_excluded_handle != NULL)
                {
                    if (thread == cur_excluded_handle)
                    {
                        handle_checked = false;
                        break;
                    }
                }
                p_cur_queue_item = p_cur_queue_item->p_next;
            }
            if (handle_checked)
            {
                timeout_tick_res = timeout_tick;
                break;
            }
        }
        else//other timeout type(e.g. workqueue item)
        {
            timeout_tick_res = timeout_tick;
            break;
        }
    }

    *ret = timeout_tick_res;
    return true;
}

bool os_pm_return_to_idle_task_zephyr(void)
{
    arch_kernel_init();//perform arm v81mainline initialization: including fault exception init & msp setting.

    NVIC_SetPriority(PendSV_IRQn, 0xff);
    NVIC_SetPriority(SysTick_IRQn, 0xff);

    uint32_t z_idle_stack_ptr;
    struct k_thread *thread = &z_idle_threads[0];

    z_idle_stack_ptr = thread->stack_info.start + thread->stack_info.size - thread->stack_info.delta;

    __set_PSP(z_idle_stack_ptr);
    __ISB();

    __set_CONTROL(__get_CONTROL() | BIT1);
    __ISB();

    extern void z_thread_entry(k_thread_entry_t, void *, void *, void *);
    extern void idle(void *, void *, void *);
    z_thread_entry(idle, 0, 0, 0);

    return true;
}

/* ************************************************* OSIF PATCH ************************************************* */
void osif_mem_func_init_zephyr()
{
    os_heap_init_zephyr();
    patch_osif_os_mem_alloc_intern         = os_mem_alloc_intern_zephyr;
    patch_osif_os_mem_zalloc_intern        = os_mem_zalloc_intern_zephyr;
    patch_osif_os_mem_aligned_alloc_intern = os_mem_aligned_alloc_intern_zephyr;
    patch_osif_os_mem_free                 = os_mem_free_zephyr;
    patch_osif_os_mem_aligned_free         = os_mem_aligned_free_zephyr;
    patch_osif_os_mem_peek                 = os_mem_peek_zephyr;
}

void osif_msg_func_init_zephyr()
{
    patch_osif_os_msg_queue_create_intern = os_msg_queue_create_intern_zephyr;
    patch_osif_os_msg_queue_delete_intern = os_msg_queue_delete_intern_zephyr;
    patch_osif_os_msg_queue_peek_intern = os_msg_queue_peek_intern_zephyr;
    patch_osif_os_msg_send_intern = os_msg_send_intern_zephyr;
    patch_osif_os_msg_recv_intern = os_msg_recv_intern_zephyr;
}

void osif_sched_func_init_zephyr(void)
{
    patch_osif_os_delay = os_delay_zephyr;
    patch_osif_os_sys_time_get = os_sys_time_get_zephyr;
    patch_osif_os_sched_start = os_sched_start_zephyr;
    patch_osif_os_sched_stop = os_sched_stop_zephyr;
    patch_osif_os_sched_suspend = os_sched_suspend_zephyr;
    patch_osif_os_sched_resume = os_sched_resume_zephyr;
    patch_osif_os_sched_state_get = os_sched_state_get_zephyr;
}

void osif_sync_func_init_zephyr(void)
{
    patch_osif_os_lock = os_lock_zephyr;
    patch_osif_os_unlock = os_unlock_zephyr;
    patch_osif_os_sem_create = os_sem_create_zephyr;
    patch_osif_os_sem_delete = os_sem_delete_zephyr;
    patch_osif_os_sem_take = os_sem_take_zephyr;
    patch_osif_os_sem_give = os_sem_give_zephyr;
    patch_osif_os_mutex_create = os_mutex_create_zephyr;
    patch_osif_os_mutex_delete = os_mutex_delete_zephyr;
    patch_osif_os_mutex_take = os_mutex_take_zephyr;
    patch_osif_os_mutex_give = os_mutex_give_zephyr;
}

void osif_task_func_init_zephyr(void)
{
    patch_osif_os_task_create = os_task_create_zephyr;
    patch_osif_os_task_delete = os_task_delete_zephyr;
    patch_osif_os_task_suspend = os_task_suspend_zephyr;
    patch_osif_os_task_resume = os_task_resume_zephyr;
    patch_osif_os_task_yield = os_task_yield_zephyr;
    patch_osif_os_task_handle_get = os_task_handle_get_zephyr;
    patch_osif_os_task_priority_get = os_task_priority_get_zephyr;
    patch_osif_os_task_priority_set = os_task_priority_set_zephyr;
    patch_osif_os_task_signal_create = os_task_signal_create_zephyr;
    patch_osif_os_task_signal_send = os_task_signal_send_zephyr;
    patch_osif_os_task_signal_recv = os_task_signal_recv_zephyr;
    patch_osif_os_task_signal_clear = os_task_signal_clear_zephyr;
}

void osif_timer_func_init_zephyr(void)
{
    patch_osif_os_timer_id_get = os_timer_id_get_zephyr;
    patch_osif_os_timer_create =  os_timer_create_zephyr;
    patch_osif_os_timer_start = os_timer_start_zephyr;
    patch_osif_os_timer_restart = os_timer_restart_zephyr;
    patch_osif_os_timer_stop = os_timer_stop_zephyr;
    patch_osif_os_timer_delete = os_timer_delete_zephyr;
    patch_osif_os_timer_state_get = os_timer_state_get_zephyr;
    patch_osif_os_timer_handle_get = os_timer_handle_get_zephyr;
    patch_osif_os_timer_auto_reload_get = os_timer_auto_reload_get_zephyr;
}

void osif_pm_func_init_zephyr(void)
{
    patch_osif_os_timer_pend_function_call = os_timer_pend_function_call_zephyr;
    patch_osif_os_pm_store = os_pm_store_zephyr;
    patch_osif_os_pm_restore = os_pm_restore_zephyr;
    patch_osif_os_systick_handler = os_systick_handler_zephyr;
    patch_osif_os_sys_tick_increase = os_sys_tick_increase_zephyr;
    patch_osif_os_sys_tick_rate_get = os_sys_tick_rate_get_zephyr;
    patch_osif_os_sys_tick_clk_get = os_sys_tick_clk_get_zephyr;
    patch_osif_os_pm_find_nearest_timeout_tick = os_pm_find_nearest_timeout_tick_zephyr;
    patch_osif_os_pm_return_idle_task = os_pm_return_to_idle_task_zephyr;
}

void os_zephyr_patch_init(void)
{
    patch_RamVectorTableUpdate = RamVectorTableUpdate_zephyr;
    osif_mem_func_init_zephyr();
    osif_msg_func_init_zephyr();
    osif_sched_func_init_zephyr();
    osif_sync_func_init_zephyr();
    osif_task_func_init_zephyr();
    osif_timer_func_init_zephyr();
    osif_pm_func_init_zephyr();
}