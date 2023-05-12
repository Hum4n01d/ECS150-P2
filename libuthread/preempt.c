/*
This preemption API is not meant to be exposed to user threads, it should stay
completely transparent for them. Whenever the user code of a thread is running,
preemption should be enabled. What does this mean?

Means that only first uthread_run has preempt told and global variable it so not
each thread able to change it.

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
#define MICROSECOND_CONVERSION 100

struct sigaction sa, old_sa;
struct itimerval timer, old_timer;
sigset_t block_signal_set;

void interrupt_handler(int signum) {
    (void)signum;

    // Yield to next thread
    uthread_yield();
}

void preempt_start(bool preempt) {
    if (!preempt) return;

    // Initialize block signal set
    sigemptyset(&block_signal_set);
    sigaddset(&block_signal_set, SIGVTALRM);

    // Set up SIGVTALRM signal handler
    sa.sa_handler = interrupt_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, &old_sa);

    // Configure timer to expire every 10 ms (100 times per second)
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = HZ * MICROSECOND_CONVERSION;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = HZ * MICROSECOND_CONVERSION;

    // Store previous timer configuration  
    setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
}

void preempt_stop(void) {
    printf("PREEMPTION STOPPED\n");

    // Restore previous action associated to virtual alarm signals
    sigaction(SIGVTALRM, &old_sa, NULL);

    // Restore previous timer configuration
    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);
}

void preempt_enable(void) {
    // Unblock SIGVTALRM
    sigprocmask(SIG_UNBLOCK, &block_signal_set, NULL);
}

void preempt_disable(void) {
    // Block SIGVTALRM
    sigprocmask(SIG_BLOCK, &block_signal_set, NULL);
}
