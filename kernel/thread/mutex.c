#include <moonos/thread/mutex.h>
#include <moonos/thread/thread.h>
#include <moonos/util/list.h>

struct mutex_wait {
    list_head_t ll;
    thread_t* thread;
};

void mutex_setup(struct mutex* mutex) {
    spin_setup(&mutex->lock);
    list_init(&mutex->wait);
    mutex->owner = 0;
}

void mutex_lock(struct mutex* mutex) {
    thread_t* me = thread_current();
    spin_lock(&mutex->lock);
    if (!mutex->owner) {
        mutex->owner = me;
        spin_unlock(&mutex->lock);
        return;
    }

    while (mutex->owner != me) {
        struct mutex_wait wait;

        wait.thread = me;
        list_add_tail(&wait.ll, &mutex->wait);
        thread_block();
        spin_unlock(&mutex->lock);

        schedule();

        spin_lock(&mutex->lock);
        list_del(&wait.ll);
    }
    spin_unlock(&mutex->lock);
}

void mutex_unlock(struct mutex* mutex) {
    spin_lock(&mutex->lock);
    if (!list_empty(&mutex->wait)) {
        struct mutex_wait* wait = (struct mutex_wait*)mutex->wait.next;

        // wake up the next thread in the queue
        mutex->owner = wait->thread;
        thread_wake(wait->thread);
    } else {
        mutex->owner = 0;
    }

    spin_unlock(&mutex->lock);
}