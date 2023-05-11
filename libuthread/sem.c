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
    int sem_count;
    queue_t sem_waiting_queue;
};

sem_t sem_create(size_t count) {
    struct semaphore* created_semaphore =
        (struct semaphore*)malloc(sizeof(struct semaphore));

    // Handle memory allocation error
    if (created_semaphore == NULL) return NULL;

    // Initialize semaphore
    created_semaphore->sem_count = count;
    created_semaphore->sem_waiting_queue = queue_create();

    return created_semaphore;
}

int sem_destroy(sem_t sem) {
    // Sem is NULL or it still has threads waiting on it.
    if ((sem == NULL) || (queue_length(sem->sem_waiting_queue) != 0)) return -1;

    // No need to free semaphore waiting queue as length is supposed to be 0
    queue_destroy(sem->sem_waiting_queue);

    // Free semaphore
    free(sem);
    sem = NULL;

    return 0;
}

int sem_down(sem_t sem) {
    if (sem == NULL) return -1;

    preempt_disable();

    // Block thread if semaphore count is 0
    while (sem->sem_count == 0) {
        struct uthread_tcb* current_uthread = uthread_current();

        // Add thread to semaphore waiting queue
        queue_enqueue(sem->sem_waiting_queue, current_uthread);

        // Switch context to next thread
        uthread_block();
    }

    // Decrement semaphore count to indicate that a thread has been blocked
    sem->sem_count -= 1;

    preempt_enable();

    return 0;
}

int sem_up(sem_t sem) {
    if (sem == NULL) return -1;

    preempt_disable();

    // Increment semaphore count to indicate that a thread has been unblocked
    sem->sem_count += 1;

    // Dequeue next thread to be executed
    struct uthread_tcb* next_uthread = NULL;
    queue_dequeue(sem->sem_waiting_queue, (void**)&next_uthread);

    // Switch context to next thread
    if (next_uthread) uthread_unblock(next_uthread);

    preempt_enable();

    return 0;
}
