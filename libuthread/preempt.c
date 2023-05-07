/*
This preemption API is not meant to be exposed to user threads, it should stay
completely transparent for them. Whenever the user code of a thread is running,
preemption should be enabled. What does this mean?

* files modified only
*/
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction sa;

void interrupt_handler(int signum) {
    /* User-defined function*/
    uthread_yield();
}

void preempt_disable(void) { /* TODO Phase 4 */
}

void preempt_enable(void) { /* TODO Phase 4 */
}

void preempt_start(bool preempt) {
    /* TODO Phase 4 */
    sa = {0};
    sa.sa_handler = interrupt_handler;
    sigaction(SIGVTALRM, &sa, NULL);
}

void preempt_stop(void) { /* TODO Phase 4 */
}
