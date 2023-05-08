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
bool global_preempt = false;

void interrupt_handler(int signum) {
    // Yield to next thread
    uthread_yield();
}

void preempt_start(bool preempt) {
    // Update global preempt flag
    global_preempt = preempt;

    if (!global_preempt) return;

    // Set up signal handler
    sa = {0};
    sa.sa_handler = interrupt_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    // Configure timer to expire every 10 ms (100 times per second)
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;

    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

/*
 * preempt_stop - Stop thread preemption
 *
 * Restore previous timer configuration, and previous action associated to
 * virtual alarm signals.
 */
void preempt_stop(void) {
    if (!global_preempt) return;

    // Restore previous timer configuration
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    // Disable timer
    setitimer(ITIMER_VIRTUAL, &timer, NULL);

    // TODO: Restore previous action associated to virtual alarm signals
}

/*
The two other functions that you must implement are meant to enable or disable
preemption. For that, you will need to be able to block or unblock signals of
type SIGVTALRM.
*/

void preempt_enable(void) {
    if (!global_preempt) return;

    sigset_t sigset;

    // Initialize signal set
    sigemptyset(&sigset);

    // Add SIGVTALRM to signal set
    sigaddset(&sigset, SIGVTALRM);

    // Unblock SIGVTALRM
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    /
}

void preempt_disable(void) {
    if (!global_preempt) return;

    sigset_t sigset;

    // Initialize signal set
    sigemptyset(&sigset);

    // Add SIGVTALRM to signal set
    sigaddset(&sigset, SIGVTALRM);

    // Block SIGVTALRM
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}
