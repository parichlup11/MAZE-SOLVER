#include "queue.h"

#include <assert.h>
#include <stdlib.h>

/*
 * Queue implementation using a linked list.
 * Used for BFS algorithm to store positions to visit.
 */

// Initializes the queue pointers to NULL
void queue_init(struct queue *q)
{
    assert(q != NULL);
    q->first = NULL;
    q->last = NULL;
}

// Returns true if the queue contains no elements
bool queue_is_empty(struct queue *q)
{
    assert(q != NULL);
    return q->first == NULL;
}

// Frees all memory allocated for queue nodes to prevent memory leaks
void queue_free(struct queue *q)
{
    assert(q != NULL);
    while (q->first != NULL) {
        struct node *next = q->first->next;
        free(q->first);
        q->first = next;
    }
    q->last = NULL;
}

// Adds a new element to the end of the queue
// Returns false if memory allocation fails
bool queue_insert(struct queue *q, struct node insert_node)
{
    assert(q != NULL);
    struct node *new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) {
        return false;
    }

    // Allocate memory for the new node
    *new_node = insert_node;
    new_node->next = NULL;

    // If queue is empty, new node is also the first one
    if (q->first == NULL) {
        q->first = new_node;
    }

   // Link the old last node to the new one
    if (q->last != NULL) {
        q->last->next = new_node;
    }
    q->last = new_node;
    return true;
}

// Removes and returns the element from the front of the queue
struct node queue_pop(struct queue *q)
{
    assert(q != NULL);
    assert(q->first != NULL); // Cannot pop from empty queue

    // Save the data to return later
    struct node result = *(q->first);
    struct node *pop_node = q->first;

    // If this was the last element, update the last pointer as well
    if (q->first == q->last) {
        q->last = NULL;
    }

    // Move head pointer to the next element
    q->first = q->first->next;
    free(pop_node); // Free the memory of the popped node
    return result;
}
