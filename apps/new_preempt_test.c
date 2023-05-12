#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uthread.h>

// multiple threads it so logic context switch so cached values not updated
// others one and so tells compiler not cache value
// m printf in kernel time and not contribute virtual
// all lines hit and signal handler and switch context fine
volatile bool flag;

void busy_loop(void) {
    while (flag) {
        printf("Inside busy loop\n");
    }
}

void thread_func2(void *arg) {
    (void)arg;

    printf("Thread 2 starting...\n");
    flag = false;
    printf("Thread 2 done.\n");
}

void thread_func1(void *arg) {
    (void)arg;

    uthread_create(thread_func2, NULL);

    printf("Thread 1 starting...\n");
    flag = true;
    busy_loop();
    printf("Thread 1 done.\n");
}

int main(void) {
    uthread_run(true, thread_func1, NULL);

    printf("Test completed.\n");

    return 0;
}