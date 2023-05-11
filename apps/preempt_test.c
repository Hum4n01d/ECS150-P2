#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <uthread.h>

#define NUM_ITERATIONS 1000

int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

static void thread3(void *arg) {
    (void)arg;

    printf("thread3 start\n");

    while (true) {
        counter3++;

        // no yield
    }

    printf("thread3 end\n");
}

static void thread2(void *arg) {
    (void)arg;

    printf("thread2 start\n");

    uthread_create(thread3, NULL);

    while (counter2 < 10) {
        counter2++;

        printf("thread2 counter2: %d\n", counter2);

        // No yield
    }

    printf("thread2 end\n");
}

static void thread1(void *arg) {
    (void)arg;

    printf("thread1 start\n");

    uthread_create(thread2, NULL);

    int i;
    for (i = 0; i < 10; i++) {
        counter1++;

        printf("thread1 counter1: %d\n", counter1);

        // Yield to next thread
        uthread_yield();
    }

    printf("thread1 end\n");
}

int main(void) {
    uthread_run(true, thread1, NULL);

    return 0;
}