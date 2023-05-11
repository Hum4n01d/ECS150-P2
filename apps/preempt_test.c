#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

void busy_loop(void) {
    intmax_t i = 0;
    while (i < 10000000000) {
        i++;
    }
}

void thread_func2(void *arg) {
    (void)arg;

    printf("Thread 2 starting...\n");
    busy_loop();
    printf("Thread 2 done.\n");
}

void thread_func1(void *arg) {
    (void)arg;

    uthread_create(thread_func2, NULL);

    printf("Thread 1 starting...\n");
    for (int i = 0; i < 5; i++) {
        printf("Thread 1 running...\n");
        uthread_yield();
    }
    printf("Thread 1 done.\n");
}

int main(void) {
    uthread_run(true, thread_func1, NULL);

    printf("Test complete.\n");

    return 0;
}