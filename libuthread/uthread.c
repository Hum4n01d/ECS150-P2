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
    void* thread_stack_top;
    uthread_ctx_t* thread_context;
};

struct uthread_tcb* uthread_current(void) { return currently_executing_thread; }

void uthread_yield(void) {
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;

    // Place current thread in ready queue to be resumed later
    queue_enqueue(ready_queue, current_uthread);

    // Dequeue next thread to be executed
    queue_dequeue(ready_queue, (void*)&next_uthread);

    // Switch context to next thread
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

void uthread_exit(void) {
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;

    // Place current thread in zombie queue to be deleted later
    queue_enqueue(zombie_queue, current_uthread);

    // Dequeue next thread to be executed
    queue_dequeue(ready_queue, (void*)&next_uthread);

    // Switch context to next thread
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

int uthread_create(uthread_func_t func, void* arg) {
    struct uthread_tcb* thread =
        (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

    // Handle memory allocation error
    if (thread == NULL) return -1;

    // Add thread to ready queue
    queue_enqueue(ready_queue, (void*)(thread));

    // Initialize thread
    thread->thread_stack_top = uthread_ctx_alloc_stack();
    thread->thread_context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

    // Context creation status determines thread creation status
    return uthread_ctx_init(thread->thread_context, thread->thread_stack_top,
                            func, arg);
}

void uthread_destroy(queue_t queue, struct uthread_tcb* thread) {
    // Use queue to avoid unused parameter warning
    (void)queue;
    if (!thread) {
        return;
    }

    free(thread->thread_context);
    thread->thread_context = NULL;

    uthread_ctx_destroy_stack(thread->thread_stack_top);
    thread->thread_stack_top = NULL;

    free(thread);
    thread = NULL;
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
    preempt_start(preempt);

    // Initialize ready and zombie queues
    ready_queue = queue_create();
    zombie_queue = queue_create();

    /* There is no need to initialize currently executing thread because as
     * soon as we perform uthread_run(), we also perform uthread_yield() as
     * queue_length is more than or equal to 1 everytime*/
    currently_executing_thread =
        (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
    currently_executing_thread->thread_context =
        (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

    // Handle unsuccessful thread creation
    if (uthread_create(func, arg) == -1) return -1;

    // Main loop, runs until all threads have exited
    while (queue_length(ready_queue)) {
        uthread_yield();
        // Destroy all threads in zombie queue
        queue_iterate(zombie_queue, (queue_func_t)uthread_destroy);
        queue_iterate(zombie_queue, (queue_func_t)queue_delete);
    }

    // Free the memory for currently active thread
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
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;

    // Dequeue next thread to be executed
    queue_dequeue(ready_queue, (void*)&next_uthread);

    // Switch context to next thread
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

void uthread_unblock(struct uthread_tcb* uthread) {
    // Place thread back in ready queue
    queue_enqueue(ready_queue, uthread);
}
