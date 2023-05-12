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

    // Queue exists
    TEST_ASSERT(q != NULL);

    // Queue can be destroyed
    TEST_ASSERT(queue_destroy(q) == 0);
}

void test_destroy_nonempty(void) {
    fprintf(stderr, "\n*** TEST destroy nonempty ***\n");

    queue_t q = queue_create();
    int data = 3;
    queue_enqueue(q, &data);
    
    // Queue cannot be destroyed if it is not empty
    TEST_ASSERT(queue_destroy(q) == -1);
}

void test_destroy_null(void) {
    fprintf(stderr, "\n*** TEST destroy null ***\n");

    // Queue cannot be destroyed if it is NULL
    TEST_ASSERT(queue_destroy(NULL) == -1);
}

void test_enqueue_dequeue_simple(void) {
    fprintf(stderr, "\n*** TEST test enqueue dequeue simple ***\n");

    queue_t q  = queue_create();
    int data = 3;
    int *ptr;

    // Enqueue data
    TEST_ASSERT(queue_enqueue(q, &data) == 0);

    // Dequeue data
    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == 0);

    // Check that data is the same
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

    // Dequeue each item and check that it is the same as the original
    for (int i = 0; i < 5; i++) {
        queue_dequeue(q, (void **)&ptr);
        TEST_ASSERT(ptr == &data[i]);
    }
}

void test_dequeue_empty(void) {
    fprintf(stderr, "\n*** TEST dequeue empty ***\n");

    queue_t q = queue_create();
    int *ptr;

    // Dequeue from empty queue should return -1
    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);
}

void test_dequeue_to_empty(void) {
    fprintf(stderr, "\n*** TEST dequeue to empty ***\n");

    queue_t q = queue_create();
    int data = 3;
    int *ptr;
    queue_enqueue(q, &data);
    queue_dequeue(q, (void **)&ptr);

    // Dequeue from empty queue should return -1
    TEST_ASSERT(ptr == &data);
    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);
}

/* Test callback function that increments items and deletes item '42' */
static void iterator_inc(queue_t q, void *data) {
    (void)q;  // unused parameter

    int *a = (int *)data;
    *a += 1;
}

void test_iterator(void) {
    fprintf(stderr, "\n*** TEST queue_iterate ***\n");

    queue_t q = queue_create();
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    int data_original[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    // Increment every item of the queue
    queue_iterate(q, iterator_inc);

    // Check that every item was incremented
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        int *ptr;
        queue_dequeue(q, (void **)&ptr);
        TEST_ASSERT(*ptr == data_original[i] + 1);
    }
}

void test_delete(void) {
    fprintf(stderr, "\n*** TEST queue_delete ***\n");
    
    queue_t q = queue_create();
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    // Delete item '42'
    TEST_ASSERT(queue_delete(q, &data[5]) == 0);

    // Check that item '42' is no longer in the queue
    TEST_ASSERT(queue_delete(q, &data[5]) == -1);
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
    
    queue_t q = queue_create();
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    // Check that the queue length is correct
    TEST_ASSERT(queue_length(q) == 10);
    
    // Remove '42'
    TEST_ASSERT(queue_delete(q, &data[5]) == 0);

    // Check that the queue length is still correct
    TEST_ASSERT(queue_length(q) == 9);
}

int main(void) {
    test_create();
    test_destroy();
    test_destroy_nonempty();
    test_destroy_null();
    test_enqueue_dequeue_simple();
    test_enqueue_multiple();
    test_dequeue_empty();
    test_dequeue_to_empty();
    test_iterator();
    test_delete();
    test_delete_null();
    test_queue_length();

    fprintf(stderr, "\n*** ALL TESTS PASSED ***\n");

    return 0;
}
