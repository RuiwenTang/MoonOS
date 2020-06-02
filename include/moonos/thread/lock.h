#ifndef __LOCK_H__
#define __LOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

struct spinlock {
    int dummy;
};

void spin_setup(struct spinlock* lock);

/**
 * @brief Use this to protect data is cannot be accessed from an interrupt
 * handler
 *
 */
void spin_lock(struct spinlock* lock);
void spin_unlock(struct spinlock* lock);

/**
 * @brief use this to protect data that can be accessed from an interrupt
 * handler.
 *
 */
int spin_lock_int_save(struct spinlock* lock);
void spin_unlock_int_restore(struct spinlock* lock, int enable);

#ifdef __cplusplus
}
#endif

#endif  // __LOCK_H__