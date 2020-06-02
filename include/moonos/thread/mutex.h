#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <moonos/thread/lock.h>
#include <moonos/util/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mutex guaranties mutual exclusion as spinlock does, but if a
 * thread tries to acquire a mutex while it's being held by
 * another thread the thread trying to acquire the mutex will
 * be blocked.
 **/
struct thread;

struct mutex {
    struct spinlock lock;
    list_head_t wait;
    struct thread* owner;
};

void mutex_setup(struct mutex* mutex);

void mutex_lock(struct mutex* mutex);
void mutex_unlock(struct mutex* mutex);

#ifdef __cplusplus
}
#endif

#endif  // __MUTEX_H__