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

/*
 * Create
 */
void test_create(void) {
    fprintf(stderr, "\n*** TEST create ***\n");

    TEST_ASSERT(queue_create() != NULL);
}

/*
 * Destroy
 */
void test_destroy(void) {
    fprintf(stderr, "\n*** TEST destroy ***\n");

    queue_t q = queue_create();

    // Queue exists
    TEST_ASSERT(q != NULL);

    // Queue can be destroyed
    TEST_ASSERT(queue_destroy(q) == 0);
}

void test_destroy_nonempty_queue(void) {
    fprintf(stderr, "\n*** TEST destroy nonempty queue ***\n");

    queue_t q = queue_create();
    int data = 3;
    queue_enqueue(q, &data);

    // Queue cannot be destroyed if it is not empty
    TEST_ASSERT(queue_destroy(q) == -1);
}

void test_destroy_null_queue(void) {
    fprintf(stderr, "\n*** TEST destroy null queue ***\n");

    // Queue cannot be destroyed if it is NULL
    TEST_ASSERT(queue_destroy(NULL) == -1);
}

/*
 * Enqueue
 */
void test_enqueue(void) {
    fprintf(stderr, "\n*** TEST test enqueue ***\n");

    queue_t q = queue_create();
    int data = 3;

    // Enqueue data
    TEST_ASSERT(queue_enqueue(q, &data) == 0);
}

void test_enqueue_multiple(void) {
    fprintf(stderr, "\n*** TEST test enqueue multiple ***\n");

    queue_t q = queue_create();
    int data[] = {1, 2, 3, 4, 5};
    int *ptr;
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(queue_enqueue(q, &data[i]) == 0);
    }

    // Dequeue each item and check that it is the same as the original
    for (int i = 0; i < 5; i++) {
        queue_dequeue(q, (void **)&ptr);
        TEST_ASSERT(ptr == &data[i]);
    }
}

void test_enqueue_null_queue(void) {
    fprintf(stderr, "\n*** TEST enqueue to null ***\n");

    int data = 3;

    // Enqueue to NULL should return -1
    TEST_ASSERT(queue_enqueue(NULL, &data) == -1);
}

void test_enqueue_null_data(void) {
    fprintf(stderr, "\n*** TEST enqueue null data ***\n");

    queue_t q = queue_create();

    // Enqueue NULL data should return -1
    TEST_ASSERT(queue_enqueue(q, NULL) == -1);
}

/*
 * Dequeue
 */
void test_dequeue(void) {
    fprintf(stderr, "\n*** TEST test dequeue ***\n");

    queue_t q = queue_create();
    int data = 3;
    int *ptr;

    // Enqueue data
    TEST_ASSERT(queue_enqueue(q, &data) == 0);

    // Dequeue data
    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == 0);

    // Check that data is the same
    TEST_ASSERT(ptr == &data);
}

void test_dequeue_multiple(void) {
    fprintf(stderr, "\n*** TEST test enqueue multiple ***\n");

    queue_t q = queue_create();
    int data[] = {1, 2, 3, 4, 5};
    int *ptr;
    for (int i = 0; i < 5; i++) {
        queue_enqueue(q, &data[i]);
    }

    // Dequeue each item and check that it is the same as the original
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == 0);
    }
}

void test_dequeue_null_queue(void) {
    fprintf(stderr, "\n*** TEST dequeue null queue ***\n");

    int *ptr;

    // Dequeue from NULL should return -1
    TEST_ASSERT(queue_dequeue(NULL, (void **)&ptr) == -1);
}

void test_dequeue_to_null_data(void) {
    fprintf(stderr, "\n*** TEST dequeue to null ***\n");

    queue_t q = queue_create();

    // Dequeue into NULL should return -1
    TEST_ASSERT(queue_dequeue(q, NULL) == -1);
}

void test_dequeue_from_empty_queue(void) {
    fprintf(stderr, "\n*** TEST dequeue from empty queue ***\n");

    queue_t q = queue_create();
    int *ptr;

    // Dequeue from empty queue should return -1
    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);
}

/*
 * Delete
 */
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

void test_delete_from_null_queue(void) {
    fprintf(stderr, "\n*** TEST delete from null queue ***\n");

    int data = 1;

    // Delete from NULL queue should return -1
    TEST_ASSERT(queue_delete(NULL, &data) == -1);
}

void test_delete_null_data(void) {
    fprintf(stderr, "\n*** TEST queue_delete_null_data ***\n");

    queue_t q = queue_create();

    // Delete NULL data from queue
    TEST_ASSERT(queue_delete(q, NULL) == -1);
}

void test_delete_not_in_queue(void) {
    queue_t q;
    int data = 1;

    fprintf(stderr, "\n*** TEST delete data not in queue ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();

    /* Test deleting a node that isn't in the queue */
    TEST_ASSERT(queue_delete(q, &data) == -1);
}

/*
 * Iterate
 */
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
    TEST_ASSERT(queue_iterate(q, iterator_inc) == 0);

    // Check that every item was incremented
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        int *ptr;
        queue_dequeue(q, (void **)&ptr);
        TEST_ASSERT(*ptr == data_original[i] + 1);
    }
}

void test_iterate_null_queue(void) {
    fprintf(stderr, "\n*** TEST iterate null queue ***\n");

    // Iterate over NULL queue should return -1
    TEST_ASSERT(queue_iterate(NULL, iterator_inc) == -1);
}

void test_iterate_null_func(void) {
    fprintf(stderr, "\n*** TEST iterate null func ***\n");

    queue_t q = queue_create();

    // Iterate over queue with NULL function should return -1
    TEST_ASSERT(queue_iterate(q, NULL) == -1);
}

/*
 * Length
 */
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

void test_queue_length_null(void) {
    fprintf(stderr, "\n*** TEST queue length null ***\n");

    // Length of NULL queue should return -1
    TEST_ASSERT(queue_length(NULL) == -1);
}

int main(void) {
    // Test create
    test_create();

    // Test destroy
    test_destroy();
    test_destroy_nonempty_queue();
    test_destroy_null_queue();

    // Test enqueue
    test_enqueue();
    test_enqueue_multiple();
    test_enqueue_null_queue();
    test_enqueue_null_data();

    // Test dequeue
    test_dequeue();
    test_dequeue_multiple();
    test_dequeue_null_queue();
    test_dequeue_to_null_data();
    test_dequeue_from_empty_queue();

    // Test delete
    test_delete();
    test_delete_from_null_queue();
    test_delete_null_data();
    test_delete_not_in_queue();

    // Test iterate
    test_iterator();
    test_iterate_null_queue();
    test_iterate_null_func();

    // Test length
    test_queue_length();
    test_queue_length_null();

    fprintf(stderr, "\n*** ALL TESTS PASSED ***\n");

    return 0;
}
