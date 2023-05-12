# ECS 150: Project #2 - User-level thread library

**Table of Contents**

- [ECS 150: Project #2 - User-level thread
  library](#ecs-150-project-2---user-level-thread-library)
  - [Queue](#queue)
  - [Thread](#thread)
  - [Semaphore](#semaphore)
  - [Preemption](#preemption)
  - [Testing](#testing)
    - [Queue](#queue-1)
    - [Thread](#thread-1)
    - [Semaphore](#semaphore-1)
    - [Preemption](#preemption-1)
  - [Debugging](#debugging)
  - [Acknowledgements](#acknowledgements)

## Queue

We chose to implement the `queue` using a linked list with head and tail
pointer. This is because we need to be able to add and remove elements from the
queue in constant time. A linked list allows us to do this by simply updating
the pointers of the nodes in O(1) time.

Each `node` in the linked list contains a pointer to the next node and a pointer
to the data. The data is a void pointer, which allows us to store any type of
data in the queue.

The queue itself contains a pointer to the head and tail of the queue, as well
as the length of the queue. The length is stored in the queue struct to allow
for O(1) access.

**Notes on `queue_delete`:**

1. This function compares the data by address, not by value. This is because we
   want to be able to delete any data from the queue, not just data that is
   comparable. For example, if duplicate data is added to the queue, we want to
   be able to delete both instances of the data separately.
2. This function does not free the data. This is because the data is not
   allocated by the queue, and the queue does not know how to free the data. It
   is the responsibility of the user to free the data. In our case, for example,
   uthread_destroy frees the data before calling queue_delete.

**Notes on `queue_iterate`:**

1. The next node is stored in a temporary variable before calling the function.
   This is because the function may delete the current node, which would cause a
   segfault if the next node is not stored beforehand.

## Uthread

In our uthread library, we create a TCB (thread control block) struct that
stores the pointer to the top of stack and a pointer to the thread's context. We
create two global queue instances, `ready_queue` for a queue of ready threads,
`zombie_queue` for a queue of zombie threads and we maintain a global pointer to
our currently executing thread. Note that we do not need to maintain a blocked
queue because as we will see, each semaphore has a queue of threads waiting on
it. Also, the reason for making a `ready_queue` is to be able to call the
threads in a round-robin manner when preemption happens. Please note that we
need the `zombie_queue` because the current thread cannot destroy itself.

All the functions are implemented as required, so we would not dive into how we
implement them. However, there are a few things to take care of in particular.
In our `uthread_run`, we create the current thread but do not initialize it as
it is supposed to create the first thread and yield to it, where `uthread_yield`
would initialize it. Also, we have a loop that runs as long as the queue is
non-empty, and keeps on yielding to the next thread in the ready queue. However,
a very important task of `uthread_run` is to also delete the zombie queue inside
this while loop because if we wait for the entire while loop to finish, then
there might be a large number of dead threads in the zombie queue which would be
a major memory inefficiency. Also, just as a note, `uthread_yield` and
`uthread_exit` are very similar except that `uthread_yield` enques the current
thread into ready queue whereas `uthread_exit` enqueues the current thread into
zombie queue before dequeuing the next ready thread. Also, uthread_create and
`uthread_destroy` are inverses of one another in terms of memory allocation and
deallocation.

## Semaphore

We implemented semaphores using a queue to store threads that are blocked on the
semaphore. Each semaphore has a count that is incremented when a thread is
unblocked and decremented when a thread is blocked. When a thread is blocked, it
is added to the semaphore's waiting queue. When a thread is unblocked, the next
thread in the waiting queue is dequeued and unblocked.

We chose to use a queue to store the waiting threads because we need to be able
to unblock the threads in the order that they were blocked. A queue allows us to
do this by simply dequeuing the next thread in the queue.

**Important note on preemption**

We temporarily disable preemption whenever we are modifying any global variables
(like enqueing into the ready queue) because we do not want the timer handler to
be invoked while we are executing a critical section. Disabling preemption
ensures that the set of instructions happens atomically. It is important to note
that we only do this in `sem_up` and `sem_down` in `sem.c` because there should
not be a case where multiple threads are creating or destroying semaphore
concurrently. However, the use of preemption disabling and enabling is rather
non-trivial inside `uthread.c`. We do not need to do this in `uthread_block` or
`uthread_unblock` because both of these are called inside `sem_up` or `sem_down`
which already do preemption disabling. It is also quite interesting to note that
only the while loop (as referred in above section) of uthread_run is included
inside the start and stop of preemption because outside the while loop, there is
only one thread which is the main/idle thread. Simliar to sem, we realized that
there is no need to disable preemptions in uthread_destroy.

### Detailed insight

We spent multiple hours debugging a segfault that we were getting after the
correct output of sem_prime.c test case. The issue was that we were destroying
the zombie queue threads before deleting them from the queue (which
theoretically should be fine). However, based on how `ucontext.h` is
implemented, we should delete objects from the queue and then destroy them.

## Preemption

In order to implement preemption, we used the `setitimer` function to set up a
timer that would send a `SIGVTALRM` signal to the program every 10 ms (frequency
of 100Hz). We then set up a signal handler for the `SIGVTALRM` signal that would
call `uthread_yield` to yield to the next thread. This will prevent a greedy
thread from running forever and preventing other threads from running.

When preemption is enabled, the `SIGVTALRM` signal is unblocked, which allows
the signal handler to be invoked. When preemption is disabled, the `SIGVTALRM`
signal is blocked, which prevents the signal handler from being invoked.

## Testing

### Queue

We tested the queue by writing unit tests that cover all of the functions in the
queue. We tested the queue by adding and removing data from the queue and
checking that the data was added and removed correctly. We additionally wrote
tests for every error case in the queue functions to ensure that passing null
pointers or invalid data would not cause the program to crash.

### Preemption

Preemption proved to be the most difficult part of the project to test. We had
to make sure that the timer was set up correctly and that the timer handler was
being invoked at the correct times. It was difficult to determine whether
preemption was actually kicking in due to the raw execution speed of
incrementing a variable. Our solution to this problem was to introduce some
latency with print statements, but only every 100000000 iterations to prevent
the output from being too cluttered. This allowed us to see that the timer
handler was being invoked at the correct times without setting an infinite loop.

## Debugging

We utilized mostly `printf` statements to debug our code since this project
since it's a bit more difficult to debug multithreaded programs with advanced
software and this proved a bit easier, especially with preemption when the
timing of the program is important.

We also used `valgrind` to check for memory leaks and were able to identify and
fix all of them. Please note that we were asked to ignore the context errors
provided by `valgrind` by the teaching staff.

## Acknowledgements

**Timers**

https://www.gnu.org/software/libc/manual/2.36/html_mono/libc.html#Setting-an-Alarm

We referred to this link to see basic examples for setting an alarm that
triggers a `SIGVTALRM` signal. This was also helpful to learn how to store and
restore a sigaction struct.

https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-setitimer-set-value-interval-timer

We referred to the above link to learn some of the details on how the
`setitimer` updates the timer struct as time passes.

**Skeleton Code**

We used the starter code provided by Professor Porquet at
`/home/cs150jp/public/p2/`
