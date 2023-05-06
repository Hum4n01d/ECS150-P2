/*
Memory see
All errors see
*/

#include "uthread.h"

#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"

queue_t scheduling_queue;
queue_t zombie_queue;

enum state { Ready, Running, Blocked, Zombie };

struct uthread_tcb {
    /* TODO Phase 2 */
    enum state thread_state;
    void* thread_stack_top;
    uthread_ctx_t* thread_context;
};

struct uthread_tcb* uthread_current(void) { /* TODO Phase 2/3 */ }

void uthread_yield(void) {
    /* TODO Phase 2 */
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;
    queue_enqueue(current_uthread);
    queue_dequeue(scheduling_queue, &next_uthread);
    uthread_ctx_switch(current_uthread, next_uthread);
}

void uthread_exit(void) { /* TODO Phase 2 */ }

int uthread_create(uthread_func_t func, void* arg) {
    /* TODO Phase 2 */
    struct uthread_tcb* created_thread =
        (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

    /*Memory allocation error*/
    if (created_thread = NULL) return -1;

    queue_enqueue(scheduling_queue, (void*)(created_thread));
    created_thread->thread_state = Ready;
    created_thread->thread_stack_top = uthread_ctx_alloc_stack();
    int context_creation_success =
        uthread_ctx_init(created_thread->thread_context,
                         created_thread->thread_stack_top, func, arg);

    /*if context creation is successful, uthread creation is successful else if
     * context creation is not successful, then uthread creation is not
     * successful*/
    return context_creation_success;
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
    /* TODO Phase 2 */
    scheduling_queue = queue_create();
    int initial_thread_success = uthread_create(func, arg);
    while (queue_length(scheduling_queue)) {
        uthread_yield();
    }
}

void uthread_block(void) { /* TODO Phase 3 */ }

void uthread_unblock(struct uthread_tcb* uthread) { /* TODO Phase 3 */ }
