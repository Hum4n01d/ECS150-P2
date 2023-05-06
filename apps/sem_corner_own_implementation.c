/*
 * Semaphore corner test

 * Thread A calls down() on a semaphore with a count of 0, and gets blocked.
 * Thread B calls up() on the same semaphore, and gets thread A to be awaken
 * Before thread A can run again, thread C calls down() on the semaphore and
snatch the newly available resource.

There are two difficulties with this scenario:

* The semaphore should make sure that thread A will handle the situation
correctly when finally resuming its execution. Theoretically, it should go back
to sleep if the resource is not longer available by the time it gets to run. If
the thread proceeds anyway, then the semaphore implementation is incorrect.

*If this keeps happening, thread A will eventually be starving (i.e., it never
gets access to the resource that it needs to proceed). Ideally, the semaphore
implementation should prevent starvation from happening.
 */

#include <limits.h>
#include <sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

sem_t sem;

static void threadC(void *arg) {
    (void)arg;

    sem_down(sem); /* Wait for threadA */
    printf("threadC\n");
}

static void threadB(void *arg) {
    (void)arg;

    sem_up(sem); /* Wait for thread 3 */
    printf("threadB\n");
}

static void threadA(void *arg) {
    (void)arg;

    uthread_create(threadB, NULL);
    uthread_create(threadC, NULL);
    sem_down(sem); /* Wait for thread 2 */
    printf("threadA\n");
}

int main(void) {
    sem = sem_create(0);
    uthread_run(false, threadA, NULL);
    sem_destroy(sem);
    return 0;
}
