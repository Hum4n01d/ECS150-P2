#include <stdio.h>

#include "uthread.h"

#define NUM_ITERATIONS 1000

int counter1 = 0;
int counter2 = 0;

static void thread2(void *arg) {
    (void)arg;

    int i;
    for (i = 0; i < NUM_ITERATIONS; i++) {
        printf("Thread 2 iteration %d\n", i);
        counter2++;

        // No yield
    }
}

static void thread1(void *arg) {
    (void)arg;

    uthread_create(thread2, NULL);

    int i;
    for (i = 0; i < NUM_ITERATIONS; i++) {
        printf("Thread 1 iteration %d\n", i);
        counter1++;
        uthread_yield();
    }
}

int main(void) {
    preempt_start(true);

    preempt_enable();

    uthread_run(false, thread1, NULL);

    printf("Thread 1 ran %d times\n", counter1);
    printf("Thread 2 ran %d times\n", counter2);

    preempt_disable();

    preempt_stop();

    return 0;
}
