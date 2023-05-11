#include <assert.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT(assert)                 \
    do {                                    \
        printf("ASSERT: " #assert " ... "); \
        if (assert) {                       \
            printf("PASS\n");               \
        } else {                            \
            printf("FAIL\n");               \
            exit(1);                        \
        }                                   \
    } while (0)


void test_create(void) {
    fprintf(stderr, "*** TEST create ***\n");

    TEST_ASSERT(queue_create() != NULL);
}

void test_destroy(void) {
    fprintf(stderr, "*** TEST destroy ***\n");

    queue_t q = queue_create();
    TEST_ASSERT(q != NULL);

    TEST_ASSERT(queue_destroy(q) == 0);
}

void test_queue_simple(void) {
    int data = 3, *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST queue_simple ***\n");

    q = queue_create();
    queue_enqueue(q, &data);

    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data) {
    int *a = (int *)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void) {
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    fprintf(stderr, "*** TEST queue_iterate ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}

void test_delete(void) {
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    fprintf(stderr, "*** TEST queue_delete ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Test deleting a node in the queue */
    TEST_ASSERT(queue_delete(q, &data[5]) == 0);
}

void test_delete_null(void) {
    queue_t q;
    int data = 1;

    fprintf(stderr, "*** TEST queue_delete_null ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    
    /* Test deleting a node that isn't in the queue */
    TEST_ASSERT(queue_delete(q, &data) == -1);
}

int main(void) {
    test_create();
    test_queue_simple();
    test_iterator();
    test_delete();
    test_delete_null();

    return 0;
}
