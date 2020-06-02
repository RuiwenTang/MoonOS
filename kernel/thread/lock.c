#include <moonos/interupt.h>
#include <moonos/thread/lock.h>
#include <moonos/thread/thread.h>

void spin_setup(struct spinlock* lock) { (void)lock; }

void spin_lock(struct spinlock* lock) {
    (void)lock;
    preempt_disable();
}

void spin_unlock(struct spinlock* lock) {
    (void)lock;
    preempt_enable();
}

int spin_lock_int_save(struct spinlock* lock) {
    const int enabled = local_int_save();

    spin_lock(lock);
    return enabled;
}

void spin_unlock_int_restore(struct spinlock* lock, int enable) {
    spin_unlock(lock);
    local_int_restore(enable);
}