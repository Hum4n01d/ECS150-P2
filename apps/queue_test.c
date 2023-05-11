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
    fprintf(stderr, "\n*** TEST create ***\n");

    TEST_ASSERT(queue_create() != NULL);
}

void test_destroy(void) {
    fprintf(stderr, "\n*** TEST destroy ***\n");

    queue_t q = queue_create();

    TEST_ASSERT(q != NULL);
    TEST_ASSERT(queue_destroy(q) == 0);
}

void test_destroy_nonempty(void) {
    fprintf(stderr, "\n*** TEST destroy nonempty ***\n");

    queue_t q = queue_create();
    int data = 3;

    TEST_ASSERT(q != NULL);
    TEST_ASSERT(queue_enqueue(q, &data) == 0);
    TEST_ASSERT(queue_destroy(q) == -1);
}

void test_destroy_null(void) {
    fprintf(stderr, "\n*** TEST destroy null ***\n");

    TEST_ASSERT(queue_destroy(NULL) == -1);
}

void test_enqueue_simple(void) {
    fprintf(stderr, "\n*** TEST test enqueue simple ***\n");

    queue_t q  = queue_create();
    int data = 3;
    int *ptr;

    queue_enqueue(q, &data);
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data);
}

void test_enqueue_multiple(void) {
    fprintf(stderr, "\n*** TEST test enqueue multiple ***\n");

    queue_t q = queue_create();
    int data[] = {1, 2, 3, 4, 5};
    int *ptr;

    for (int i = 0; i < 5; i++) {
        queue_enqueue(q, &data[i]);
    }

    for (int i = 0; i < 5; i++) {
        queue_dequeue(q, (void **)&ptr);
        TEST_ASSERT(ptr == &data[i]);
    }
}

void test_dequeue_empty(void) {
    fprintf(stderr, "\n*** TEST dequeue empty ***\n");

    queue_t q = queue_create();
    int *ptr;

    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);
}

void test_dequeue_to_empty(void) {
    fprintf(stderr, "\n*** TEST dequeue to empty ***\n");

    queue_t q = queue_create();
    int data = 3;
    int *ptr;

    queue_enqueue(q, &data);
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data);
    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);
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

    fprintf(stderr, "\n*** TEST queue_iterate ***\n");

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

    fprintf(stderr, "\n*** TEST queue_delete ***\n");

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

    fprintf(stderr, "\n*** TEST queue_delete_null ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    
    /* Test deleting a node that isn't in the queue */
    TEST_ASSERT(queue_delete(q, &data) == -1);
}

void test_queue_length(void) {
    fprintf(stderr, "\n*** TEST queue length ***\n");

    
}

int main(void) {
    test_create();
    test_destroy();
    test_destroy_nonempty();
    test_destroy_null();
    test_enqueue_simple();
    test_enqueue_multiple();
    test_dequeue_empty();
    test_dequeue_to_empty();
    test_iterator();
    test_delete();
    test_delete_null();
    test_queue_length();

    return 0;
}
