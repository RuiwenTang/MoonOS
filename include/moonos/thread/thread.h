#ifndef __THREAD_H__
#define __THREAD_H__

#include <moonos/memory/mm.h>
#include <moonos/thread/condition.h>
#include <moonos/thread/lock.h>
#include <moonos/util/list.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum thread_state {
    THREAD_ACTIVE,
    THREAD_BLOCKED,
    THREAD_FINISHED,
    THREAD_DEAD
};
typedef enum thread_state thread_state_t;

struct thread {
    list_head_t ll;
    struct spinlock lock;
    struct condition cv;
    uintptr_t stack_phys;
    int stack_order;
    thread_state_t state;
    mm_t* mm;
    void* context;
    int retval;
};
typedef struct thread thread_t;

thread_t* __thread_create(int stack_order, int (*fptr)(void*), void* arg);
thread_t* thread_create(int (*fptr)(void*), void* arg);

void thread_start(thread_t* thread);
thread_t* thread_current(void);

void thread_exit(int ret);
void thread_join(thread_t* thread, int* ret);
void thread_destroy(thread_t* thread);
void thread_block(void);
void thread_wake(thread_t* thread);

void preempt_disable(void);
void preempt_enable(void);

void schedule(void);
void scheduler_tick(void);
void scheduler_idle(void);
void scheduler_setup(void);

#ifdef __cplusplus
}
#endif

#endif  // __THREAD_H__