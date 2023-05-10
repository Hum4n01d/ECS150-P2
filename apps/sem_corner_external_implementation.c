#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * A B C
 * B C A
 * C A D but A is blocked
*/
#include <signal.h>
#include <unistd.h>

#include <sem.h>
#include <uthread.h>

sem_t sem;
void test_handler(int signum)
{
    (void)signum;

    printf("Releasing resource\n");
    sem_up(sem);
}

static void threadD(void *arg)
{
    (void)arg;
    printf("threadD start (should come before threadA)\n");
    printf("threadD end (should come before threadA)\n");
}

static void threadC(void *arg)
{
    (void)arg;
    printf("threadC start\n");

    printf("threadC sem_down start\n");
    sem_down(sem); /* Steal sem from thread A */
    printf("threadC sem_down end\n");

    uthread_create(threadD, NULL);
    printf("threadC end\n");
}

static void threadB(void *arg)
{
    (void)arg;
    printf("threadB start\n");
    printf("threadB sem_up start\n");
    sem_up(sem);
    printf("threadB sem_up end\n");
    printf("threadB end\n");
}

static void threadA(void *arg)
{
    (void)arg;
    printf("threadA start\n");

    uthread_create(threadB, NULL);
    uthread_create(threadC, NULL);
    printf("threadA sem_down start\n");
    sem_down(sem); /* Wait for thread B */
    printf("threadA sem_down end\n");
    printf("threadA end\n");
}

int main(void)
{
    printf("main start\n");
    sem = sem_create(0);

    /* Set up handler for alarm */
    struct sigaction sa;
    sa.sa_handler = test_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    alarm(3);
    uthread_run(false, threadA, NULL);
    printf("main end\n");
    return 0;
}