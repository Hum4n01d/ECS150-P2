/*
Queue destroy but each element? Iterate over with delete?
Why blocked queue as just enter into semaphore queue, and context switch when
out?
Corner case starvation not prevented
*/
#include "sem.h"

#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"

struct semaphore {
    /* TODO Phase 3 */
    int sem_count;
    queue_t sem_waiting_queue;
};

sem_t sem_create(size_t count) {
    /* TODO Phase 3 */
    struct semaphore* created_semaphore =
        (struct semaphore*)malloc(sizeof(struct semaphore));

    if (created_semaphore == NULL) {
        return NULL;
    }

    created_semaphore->sem_count = count;
    created_semaphore->sem_waiting_queue = queue_create();
    return created_semaphore;
}

int sem_destroy(sem_t sem) {
    /* TODO Phase 3 */
    /*sem is NULL or it still has threads waiting on it.*/
    if ((sem == NULL) || (queue_length(sem->sem_waiting_queue) != 0)) {
        return -1;
    }
    queue_destroy(sem->sem_waiting_queue);
    free(sem);
    sem = NULL;
    return 0;
}

int sem_down(sem_t sem) {
    /* TODO Phase 3 */
    if (sem == NULL) {
        return -1;
    }
    preempt_disable();
    while (sem->sem_count == 0) {
        struct uthread_tcb* current_uthread = uthread_current();
        queue_enqueue(sem->sem_waiting_queue, current_uthread);
        uthread_block();
    }
    sem->sem_count -= 1;
    preempt_enable();
    return 0;
}

int sem_up(sem_t sem) {
    /* TODO Phase 3 */
    if (sem == NULL) {
        return -1;
    }
    preempt_disable();
    sem->sem_count += 1;
    struct uthread_tcb* next_uthread = NULL;
    queue_dequeue(sem->sem_waiting_queue, (void**)&next_uthread);
    if (next_uthread) {
        uthread_unblock(next_uthread);
    }
    preempt_enable();
    return 0;
}
