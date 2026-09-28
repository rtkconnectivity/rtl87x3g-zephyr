void os_zephyr_patch_init(void);

#define ACTIVE 1
#define NOT_ACTIVE 0

typedef enum
{
    SCHEDULER_SUSPENDED = 0,
    SCHEDULER_NOT_STARTED = 1,
    SCHEDULER_RUNNING = 2
} SCHEDULER_STATE;

#define CONFIG_ZEPHYR_PRI_MAX 6

struct task_signal {
    struct k_poll_signal *poll_signal;
    struct k_poll_event  *poll_event;
    int32_t            signal_results;
};

typedef enum {
  TimerOnce               = 0,          /* /< One-shot timer. */
  TimerPeriodic           = 1           /* /< Repeating timer. */
} TimerType;

struct osif_timer {
    struct k_timer ztimer; 
    TimerType type;
    uint32_t interval_ms;
    uint32_t timer_id;
    uint32_t status;
	const char *name;
};

struct osif_task {
    struct k_thread zthread; 
    struct k_poll_signal poll_signal;
    struct k_poll_event  poll_event;
    int32_t signal_results;
    void *stack_start;
	const char *name;
};

typedef void (*pend_func_t)(void *para1, uint32_t para2);
struct pend_call
{
    struct k_work work;
    pend_func_t pend_func;
    void *para1;
    uint32_t para2;
};
