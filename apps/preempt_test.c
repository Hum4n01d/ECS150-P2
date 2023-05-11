#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <signal.h>
#include <sys/time.h>

void busy_loop(void) {
    int i = 0;
    while (i < 1000000000) {
        i++;

        if (i % 100000000 == 0) {
            printf("\tThread 2 run #%d\n", i);
        }
    }
}

void thread_func1(void *arg) {
    (void)arg;

    printf("=== Thread 1 starting ===\n");
    for (int i = 0; i < 20; i++) {
        printf("Thread 1 run #%d\n", i);
        uthread_yield();
    }
    printf("=== Thread 1 done ===\n");
}

void thread_func2(void *arg) {
    (void)arg;

    uthread_create(thread_func1, NULL);

    printf("=== Thread 2 starting ===\n");
    busy_loop();
    printf("=== Thread 2 done ===\n");
}

void dummy_handler(int signum) {
    (void)signum;
}

int main(void) {
    // Create dummy SIGVTALRM handler
    struct sigaction sa;
    sa.sa_handler = dummy_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGVTALRM, &sa, NULL);

    // Create dummy timer
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);

    // Run thread_func2
    uthread_run(true, thread_func2, NULL);

    // Verify that placeholder SIGVTALRM handler was restored
    struct sigaction old_sa;
    sigaction(SIGVTALRM, NULL, &old_sa);
    if (old_sa.sa_handler != dummy_handler) {
        printf("ERROR: SIGVTALRM handler was not restored\n");
        exit(EXIT_FAILURE);
    }

    // Verify that placeholder timer was restored
    struct itimerval old_timer;
    setitimer(ITIMER_VIRTUAL, NULL, &old_timer);
    if (old_timer.it_value.tv_sec != 0 || old_timer.it_value.tv_usec != 0 ||
        old_timer.it_interval.tv_sec != 0 || old_timer.it_interval.tv_usec != 0) {
        printf("ERROR: timer was not restored\n");
        exit(EXIT_FAILURE);
    }

    printf("Test complete.\n");

    return 0;
}