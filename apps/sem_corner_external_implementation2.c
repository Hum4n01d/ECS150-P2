#include <limits.h>
#include <sem.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <uthread.h>

// A B C
// B C    A
// C D    A
// D      A
//        A

sem_t sem;
void test_handler(int signum) {
    (void)signum;
    printf("Releasing resource\n");
    sem_up(sem);
}

static void threadD(void *arg) {
    (void)arg;
    printf("threadD\n");
}

static void threadC(void *arg) {
    (void)arg;
    sem_down(sem); /* Steal sem from thread A */
    uthread_create(threadD, NULL);
    printf("threadC\n");
}

static void threadB(void *arg) {
    (void)arg;

    sem_up(sem); /* Release sem for thread A (but thread C will steal */
    printf("threadB\n");
}

static void threadA(void *arg) {
    (void)arg;
    uthread_create(threadB, NULL);
    uthread_create(threadC, NULL);
    sem_down(sem); /* Wait for thread B */
    printf("threadA\n");
}

int main(void) {
    struct sigaction sa;
    sem = sem_create(0);
    /* Set up handler for alarm */
    sa.sa_handler = test_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    /* Configure alarm */
    alarm(3);

    uthread_run(false, threadA, NULL);

    sem_destroy(sem);
    return 0;
}