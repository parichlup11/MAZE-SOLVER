#ifndef QUEUE_H
#define QUEUE_H

#include "maze.h"

#include <stdbool.h>

struct node
{
    struct node *next;
    struct position pos;
    struct node *parent;
};

struct queue
{
    struct node *first;
    struct node *last;
};
void queue_init(struct queue *q);
bool queue_is_empty(struct queue *q);

void queue_free(struct queue *q);
bool queue_insert(struct queue *q, struct node insert_node);

struct node queue_pop(struct queue *q);

#endif 
