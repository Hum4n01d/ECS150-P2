/*
Memory see
All errors see
uthread_ctx_switch error so auto handled sense and we not worry, right?
context deletion and allocation mem see
zombie delete where see
where do we use iterate()?
use global variable inside or function returned?...setting direct and reading
function we
remove this blocked queue, right?
Do we need to store the states?
Why need zombie queue, can we not just delete things when exit is called?
Otherwise, need to have parent and state stored, and then delete all children
in zombie queue with certain parent id?
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

queue_t ready_queue;
queue_t zombie_queue;
struct uthread_tcb* currently_executing_thread;

enum state { Ready, Running, Blocked, Zombie };

struct uthread_tcb {
    /* TODO Phase 2 */
    enum state thread_state;
    void* thread_stack_top;
    uthread_ctx_t* thread_context;
};

struct uthread_tcb* uthread_current(void) {
    /* TODO Phase 2/3 */
    return currently_executing_thread;
}

void uthread_yield(void) {
    /* TODO Phase 2 */
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;
    queue_enqueue(ready_queue, current_uthread);
    queue_dequeue(ready_queue, (void*)&next_uthread);
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

void uthread_exit(void) {
    /* TODO Phase 2 */
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;
    queue_enqueue(zombie_queue, current_uthread);
    queue_dequeue(ready_queue, (void*)&next_uthread);
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

int uthread_create(uthread_func_t func, void* arg) {
    /* TODO Phase 2 */
    struct uthread_tcb* created_thread =
        (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
    /*Memory allocation error*/
    if (created_thread == NULL) {
        return -1;
    }

    queue_enqueue(ready_queue, (void*)(created_thread));
    created_thread->thread_state = Ready;
    created_thread->thread_stack_top = uthread_ctx_alloc_stack();
    created_thread->thread_context =
        (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
    int context_creation_success =
        uthread_ctx_init(created_thread->thread_context,
                         created_thread->thread_stack_top, func, arg);

    /*if context creation is successful, uthread creation is successful else if
     * context creation is not successful, then uthread creation is not
     * successful*/
    return context_creation_success;
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
    preempt_start(preempt);
    /* TODO Phase 2 */
    ready_queue = queue_create();
    zombie_queue = queue_create();
    /* There is no need to initialize currently executing thread because as
     * soon as we perform uthread_run(), we also perform uthread_yield() as
     * queue_length is more than or equal to 1 everytime*/
    currently_executing_thread =
        (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
    currently_executing_thread->thread_context =
        (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
    int initial_thread_success = uthread_create(func, arg);
    if (initial_thread_success == -1) {
        return -1;
    }

    while (queue_length(ready_queue)) {
        uthread_yield();
    }

    /*Free the memory for currently active thread*/
    free(currently_executing_thread->thread_context);
    currently_executing_thread->thread_context = NULL;
    free(currently_executing_thread);
    currently_executing_thread = NULL;

    /*Free memory for the queue objects*/
    queue_destroy(ready_queue);
    queue_destroy(zombie_queue);

    if (preempt) {
        preempt_stop();
    }
    return 0;
}

void uthread_block(void) {
    /* TODO Phase 3 */
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;
    queue_dequeue(ready_queue, (void*)&next_uthread);
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

void uthread_unblock(struct uthread_tcb* uthread) {
    /* TODO Phase 3 */
    queue_enqueue(ready_queue, uthread);
}
