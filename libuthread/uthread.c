/*

Asking Questions:
Memory see for own test cases
Can't think of better ones....old action part so signal do we??

not requested so skip preemption!!

no need global?
block or setmask?


Warning for ours
uthread_ctx_switch error so auto handled sense and we not worry, right?
use global variable inside or function returned?...setting direct and reading
preempt disable and enable where in uthread.c and sem.c
All errors see do
waiting queue delete if destroy before?


preempt test case OK? and how about sleep/print issues?

Report what all write?
Queue test cases 20?

MakeFile ok?m

Error do we if sem_up and down not properly sense as waiting queue possible
stuff remains? Starvation ideation help please

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

struct uthread_tcb {
    void* thread_stack_top;
    uthread_ctx_t* thread_context;
};

struct uthread_tcb* uthread_current(void) { return currently_executing_thread; }

void uthread_yield(void) {
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;

    // Place current thread in ready queue to be resumed later
    preempt_disable();
    queue_enqueue(ready_queue, current_uthread);

    // Dequeue next thread to be executed
    queue_dequeue(ready_queue, (void*)&next_uthread);

    // Switch context to next thread
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
    preempt_enable();
}

void uthread_exit(void) {
    struct uthread_tcb* current_uthread = uthread_current();
    struct uthread_tcb* next_uthread = NULL;

    // Place current thread in zombie queue to be deleted later
    preempt_disable();
    queue_enqueue(zombie_queue, current_uthread);

    // Dequeue next thread to be executed
    queue_dequeue(ready_queue, (void*)&next_uthread);

    // Switch context to next thread
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
    preempt_enable();
}

int uthread_create(uthread_func_t func, void* arg) {
    struct uthread_tcb* thread =
        (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

    // Handle memory allocation error
    if (thread == NULL) return -1;

    // Add thread to ready queue
    preempt_disable();
    queue_enqueue(ready_queue, (void*)(thread));
    preempt_enable();

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

    // undefined behaviour pointers to free memory
    queue_delete(queue, thread);
    uthread_ctx_destroy_stack(thread->thread_stack_top);
    thread->thread_stack_top = NULL;

    free(thread->thread_context);
    thread->thread_context = NULL;

    free(thread);
}

int uthread_run(bool preempt, uthread_func_t func, void* arg) {
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

    // Start preemption
    preempt_start(preempt);

    // Main loop, runs until all threads have exited
    while (queue_length(ready_queue)) {
        uthread_yield();
        // Destroy all threads in zombie queue
        queue_iterate(zombie_queue, (queue_func_t)uthread_destroy);
    }

    // Free the memory for currently active thread
    free(currently_executing_thread->thread_context);
    currently_executing_thread->thread_context = NULL;
    free(currently_executing_thread);
    currently_executing_thread = NULL;

    /*Free memory for the queue objects*/
    queue_iterate(ready_queue, (queue_func_t)uthread_destroy);
    queue_iterate(ready_queue, (queue_func_t)queue_delete);
    queue_destroy(ready_queue);

    // better practice to do it, even though programs should end with
    // idle_thread
    queue_iterate(zombie_queue, (queue_func_t)uthread_destroy);
    queue_iterate(zombie_queue, (queue_func_t)queue_delete);
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
    // already called with preemption disabled
    queue_dequeue(ready_queue, (void*)&next_uthread);

    // Switch context to next thread
    currently_executing_thread = next_uthread;
    uthread_ctx_switch(current_uthread->thread_context,
                       next_uthread->thread_context);
}

void uthread_unblock(struct uthread_tcb* uthread) {
    // Place thread back in ready queue
    // already called with preemption disabled
    queue_enqueue(ready_queue, uthread);
}
