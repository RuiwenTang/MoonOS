#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <moonos/memory/buddy.h>
#include <moonos/memory/slab.h>
#include <moonos/thread/lock.h>
#include <moonos/thread/thread.h>
#include <stdint.h>

struct switch_frame {
    uint64_t rflags;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rip;
} __attribute__((packed));

typedef struct switch_frame switch_frame_t;

static const int TIMESLICE = 5;

static slab_cache_t cache;
static struct spinlock ready_lock;
static list_head_t ready;
static thread_t* current;
static thread_t* idle;
static int remained_time;
static int preempt_count = 0;

static void thread_place(thread_t* me) {
    spin_lock(&current->lock);
    if (current->state == THREAD_FINISHED) {
        current->state = THREAD_DEAD;
        notify_one(&current->cv);
    }
    spin_unlock(&current->lock);
    current = me;
    remained_time = TIMESLICE;
}

void thread_entry(thread_t* me, int (*fptr)(void*), void* arg) {
    thread_place(me);
    local_int_enable();

    thread_exit(fptr(arg));
}

/**
 * @brief define and implement in asm file
 *
 */
extern void __switch_threads(void**, void*);

extern void __thread_entry(void);

static void switch_threads(thread_t* from, thread_t* new) {
    __switch_threads(&from->context, new->context);
}

static thread_t* thread_alloc(void) { return slab_cache_alloc(&cache); }

static void thread_free(thread_t* thread) { slab_cache_free(&cache, thread); }

thread_t* __thread_create(int stack_order, int (*fptr)(void*), void* arg) {
    thread_t* thread = thread_alloc();
    if (!thread) {
        return thread;
    }

    thread->stack_order = stack_order;
    thread->stack_phys = buddy_alloc(stack_order);
    if (!thread->stack_phys) {
        thread_free(thread);
        return 0;
    }

    thread->mm = mm_create();
    if (thread->mm == NULL) {
        buddy_free(thread->stack_phys, stack_order);
        thread_free(thread);
        return 0;
    }

    const size_t stack_size = PAGE_SIZE << stack_order;

    char* ptr = (char*)va(thread->stack_phys);
    switch_frame_t* frame =
            (switch_frame_t*)(ptr + (stack_size - sizeof(*frame)));

    frame->rip = (uint64_t)&__thread_entry;
    frame->r15 = (uint64_t)thread;
    frame->r14 = (uint64_t)fptr;
    frame->r13 = (uint64_t)arg;
    frame->rflags = 2;

    spin_setup(&thread->lock);
    condition_setup(&thread->cv);
    thread->context = frame;
    return thread;
}

thread_t* thread_create(int (*fptr)(void*), void* arg) {
    const int DEFAULT_STACK_ORDER = 3; /* 16kb stack */
    return __thread_create(DEFAULT_STACK_ORDER, fptr, arg);
}

void thread_start(thread_t* thread) {
    const int enabled = spin_lock_int_save(&ready_lock);
    thread->state = THREAD_ACTIVE;
    list_add_tail(&(thread->ll), &ready);
    spin_unlock_int_restore(&ready_lock, enabled);
}

thread_t* thread_current(void) { return current; }

void thread_exit(int ret) {
    thread_t* me = current;
    const int enabled = spin_lock_int_save(&me->lock);

    me->retval = ret;
    me->state = THREAD_FINISHED;
    spin_unlock_int_restore(&me->lock, enabled);
    schedule();
}

void thread_join(thread_t* thread, int* ret) {
    int enabled = spin_lock_int_save(&thread->lock);

    while(thread->state != THREAD_DEAD) {
        condition_wait_spin_int(&thread->cv, &thread->lock);
    }
    spin_unlock_int_restore(&thread->lock, enabled);

    if (ret) {
        *ret = thread->retval;
    }
}

void thread_destroy(thread_t* thread) {
    buddy_free(thread->stack_phys, thread->stack_order);
    mm_release(thread->mm);
    thread_free(thread);
}

void thread_wake(thread_t* thread) { thread_start(thread); }

void thread_block(void) {
    thread_t* me = current;
    const int enabled = spin_lock_int_save(&me->lock);

    me->state = THREAD_BLOCKED;
    spin_unlock_int_restore(&me->lock, enabled);
}

void schedule(void) {
    thread_t* me = current;
    thread_t* next = 0;

    const int enabled = local_int_save();

    // check if preemptition enabled
    if (preempt_count) {
        local_int_restore(enabled);
        return;
    }

    /* check the next int the list */
    spin_lock(&ready_lock);
    if (!list_empty(&ready)) {
        next = (thread_t*)ready.next;
        list_del(&next->ll);
    }
    spin_unlock(&ready_lock);

    /**
     * if there is no next and the current thread is about to block
     * use a special idle thread
     **/
    if (me->state != THREAD_ACTIVE && !next) {
        next = idle;
    }

    if (!next) {
        thread_place(me);
        local_int_restore(enabled);
        return;
    }

    /**
     * if the current thread is still active and not the special idle
     * thread, then add it to the end of the ready queue
     **/
    spin_lock(&ready_lock);
    if (me->state == THREAD_ACTIVE && me != idle) {
        list_add_tail(&me->ll, &ready);
    }
    spin_unlock(&ready_lock);

    switch_threads(me, next);
    thread_place(me);
    local_int_restore(enabled);
}

void scheduler_tick(void) {
    if (remained_time) --remained_time;

    if (remained_time <= 0) schedule();
}

void scheduler_idle(void) {
    local_int_enable();
    while (1) {
        schedule();
        __asm__("hlt");
    }
}

void scheduler_setup(void) {
    static thread_t main;

    main.state = THREAD_ACTIVE;
    current = &main;
    idle = &main;

    spin_setup(&ready_lock);
    list_init(&ready);
    slab_cache_setup(&cache, sizeof(thread_t));
}

void preempt_disable(void) {
    const int enabled = local_int_save();
    ++preempt_count;
    local_int_restore(enabled);
}

void preempt_enable(void) {
    const int enabled = local_int_save();
    --preempt_count;
    local_int_restore(enabled);
}