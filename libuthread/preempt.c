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

// #include "private.h"
// #include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction sa;

bool global_preempt = false;

#define NUM_ITERATIONS 1000

int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

void interrupt_handler(int signum) {
    printf("interrupt_handler signum: %d\n", signum);

    // Yield to next thread
    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    exit(0);
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

    int val = setitimer(ITIMER_VIRTUAL, &timer, NULL);

    printf("return: %d\n", val);
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
    struct itimerval timer;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void preempt_enable(void) {
    if (!global_preempt) return;

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

    sigset_t sigset;

    // Initialize signal set
    sigemptyset(&sigset);

    // Add SIGVTALRM to signal set
    sigaddset(&sigset, SIGVTALRM);

    // Block SIGVTALRM
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

static void thread2(void *arg) {
    (void)arg;

    while (counter2 < 1000000000) {
        counter2++;

        // No yield
    }
}

static void thread1(void *arg) {
    (void)arg;

    int i;
    for (i = 0; i < 10; i++) {
        counter1++;
    }
}

static void thread3(void *arg) {
    (void)arg;

    int i;
    while (true) {
        counter3++;
    }
}

int main(int argc, char *argv[]) {
    preempt_start(true);

    preempt_enable();

    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    thread1(NULL);

    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    thread2(NULL);

    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    preempt_disable();

    thread3(NULL);

    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    // Wait 3 seconds
    sleep(3);

    // Re-enable preemption
    preempt_enable();

    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    preempt_stop();

    printf("===Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);
    printf("Thread 3 ran %d times===\n", counter3);

    return 0;
}