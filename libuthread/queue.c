#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
    void *data;
    struct node *next;
};

struct queue {
    struct node *head;
    struct node *tail;
    int length;
};

queue_t queue_create(void) {
    queue_t q = (queue_t)malloc(sizeof(struct queue));

    // Memory allocation error
    if (q == NULL) return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->length = 0;

    return q;
}

int queue_destroy(queue_t queue) {
    if (queue == NULL) return -1;
    if (queue->length != 0) return -1;

    free(queue);
    queue = NULL;

    return 0;
}

int queue_enqueue(queue_t queue, void *data) {
    if (queue == NULL || data == NULL) return -1;

    struct node *new_node = malloc(sizeof(struct node));

    // Memory allocation error
    if (new_node == NULL) return -1;

    new_node->data = data;
    new_node->next = NULL;

    if (queue->length == 0) {
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    queue->length++;

    return 0;
}

int queue_dequeue(queue_t queue, void **data) {
    if (queue == NULL) return -1;
    if (data == NULL) return -1;
    if (queue->length == 0) return -1;

    struct node *current_node = queue->head;

    // Return data from current node
    *data = current_node->data;

    // Remove current node from queue
    queue->head = current_node->next;
    queue->length--;
    free(current_node);
    current_node = NULL;

    return 0;
}

int queue_delete(queue_t queue, void *data) {
    if (queue == NULL) return -1;
    if (data == NULL) return -1;

    struct node *current_node = queue->head;
    struct node *previous_node = NULL;

    while (current_node != NULL) {
        if (current_node->data == data) {  // Compares data by address
            if (previous_node == NULL) {
                // Current node is head, update head to point at next node
                queue->head = current_node->next;
            } else {
                // Not head, update previous node to point at next node
                previous_node->next = current_node->next;
            }

            // Update tail if current node is tail
            if (current_node == queue->tail) {
                queue->tail = previous_node;
            }

            queue->length--;
            free(current_node);
            current_node = NULL;

            return 0;
        }

        previous_node = current_node;
        current_node = current_node->next;
    }

    // Data not found
    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func) {
    if (queue == NULL) return -1;
    if (func == NULL) return -1;

    struct node *current_node = queue->head;
    struct node *next_node = NULL;

    while (current_node != NULL) {
        next_node = current_node->next;
        func(queue, current_node->data);

        // If current node is deleted, next is already set to avoid segfault
        current_node = next_node;
    }
    return 0;
}

int queue_length(queue_t queue) {
    if (queue == NULL) return -1;

    return queue->length;
}