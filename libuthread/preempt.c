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
#include <unistd.h>

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction sa;
struct itimerval old_timer;

bool global_preempt = false;

void interrupt_handler(int signum) {
    (void)signum;

    // Yield to next thread
    uthread_yield();
}

void preempt_start(bool preempt) {
    // Update global preempt flag
    global_preempt = preempt;

    if (!global_preempt) return;

    // Set up SIGVTALRM signal handler
    sa.sa_handler = interrupt_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, NULL);

    // Configure timer to expire every 10 ms (100 times per second)
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;

    setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
}

void preempt_stop(void) {
    if (!global_preempt) return;

    // Restore previous action associated to virtual alarm signals
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, NULL);

    // Restore previous timer configuration
    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);
}

void preempt_enable(void) {
    if (!global_preempt) return;

    // printf("PREEMPTION ENABLED\n");

    sigset_t sigset;

    // Initialize signal set
    sigemptyset(&sigset);

    // Add SIGVTALRM to signal set
    sigaddset(&sigset, SIGVTALRM);

    // Unblock SIGVTALRM
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}

void preempt_disable(void) {
    if (!global_preempt) return;

    printf("PREEMPTION DISABLED\n");

    sigset_t sigset;

    // Initialize signal set
    sigemptyset(&sigset);

    // Add SIGVTALRM to signal set
    sigaddset(&sigset, SIGVTALRM);

    // Block SIGVTALRM
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}
