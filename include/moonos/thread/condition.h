#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <moonos/thread/lock.h>
#include <moonos/util/list.h>

#ifdef __cplusplus
extern "C" {
#endif

struct condition {
    struct spinlock lock;
    list_head_t wait;
};

struct mutex;

void condition_setup(struct condition* cv);

// two versions of wait: one works with spinlock, other works with mutex

void condition_wait_spin(struct condition* cv, struct spinlock* lock);
void condition_wait_spin_int(struct condition* cv, struct spinlock* lock);
void condition_wait(struct condition* cv, struct mutex* lock);

void notify_one(struct condition* cv);
void notify_all(struct condition* cv);

#ifdef __cplusplus
}
#endif

#endif  // __CONDITION_H__
