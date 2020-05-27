#ifndef __THREAD_H__
#define __THREAD_H__

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
    uintptr_t stack_phys;
    int stack_order;
    thread_state_t state;
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

void schedule(void);
void scheduler_tick(void);
void scheduler_idle(void);
void scheduler_setup(void);

#ifdef __cplusplus
}
#endif

#endif  // __THREAD_H__