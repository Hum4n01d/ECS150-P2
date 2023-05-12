/*
 * Semaphore simple test
 *
 * Test the synchronization of three threads, by having them print messages in
 * a certain order.
 */

#include <limits.h>
#include <sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

sem_t sem1;
sem_t sem2;
sem_t sem3;

static void thread3(void *arg) {
    printf("thread3 start\n");
    (void)arg;

    sem_down(sem3); /* Wait for thread1 */
    printf("thread3\n");
    sem_up(sem2); /* Unblock thread2 */
}

static void thread2(void *arg) {
    printf("thread2 start\n");
    (void)arg;

    // Attempt take from sem 2
    // Sem 2 is not available, thread 2 is added to sem 2 waiting queue
    // Thread 2 is blocked, so context switched to next thread
    sem_down(sem2);

    // This line is not executed until thread 2 is unblocked
    printf("thread2 unblocked\n");
    sem_up(sem1); /* Unblock thread1 */
}

static void thread1(void *arg) {
    (void)arg;

    uthread_create(thread2, NULL);

    uthread_create(thread3, NULL);

    sem_up(sem3);   /* Unblock thread 3 */
    sem_down(sem1); /* Wait for thread 2 */
    printf("thread1\n");
}

int main(void) {
    sem1 = sem_create(0);
    sem2 = sem_create(0);
    sem3 = sem_create(0);

    printf("run start\n");

    uthread_run(false, thread1, NULL);

    printf("destroy start\n");

    sem_destroy(sem1);
    sem_destroy(sem2);
    sem_destroy(sem3);

    return 0;
}
