#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

void busy_loop(void) {
    int i = 0;
    while (i < 1000000000) {
        i++;

        if (i % 10000000 == 0) {
            printf("Busy loop %d\n", i);
        }
    }
}

void thread_func1(void *arg) {
    (void)arg;

    printf("Thread 1 starting...\n");
    for (int i = 0; i < 5; i++) {
        printf("Thread 1 running...\n");
        uthread_yield();
    }
    printf("Thread 1 done.\n");
}

void thread_func2(void *arg) {
    (void)arg;

    uthread_create(thread_func1, NULL);

    printf("Thread 2 starting...\n");
    busy_loop();
    printf("Thread 2 done.\n");
}

int main(void) {
    uthread_run(true, thread_func2, NULL);

    printf("Test complete.\n");

    return 0;
}