#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdlib.h> // due to size_t type

typedef
struct node
{
    struct node *next;
    size_t index;
} node;

typedef
struct queue
{
    struct node *first;
    struct node *last;
} queue;

/**
 * Initializes the queue
 * 
 * @param q a pointer to struct queue
*/
void queue_init(struct queue *q);

/**
 * Checks whether the queue is empty
 * 
 * @param q a pointer to struct queue
 * 
 * @return true if queue is empty, false otherwise
*/
bool queue_is_empty(struct queue *q);

/**
 * Frees all nodes inside the queue
 * 
 * @param q a pointer to struct queue
*/
void queue_free(struct queue *q);

/**
 * Inserts a new node to the back of the queue
 * 
 * @param q a pointer to struct queue
 * @param insert_node node for insertion
 * 
 * @return true if insterted, false otherwise
*/
bool queue_insert(struct queue *q, struct node insert_node);

/**
 * Removes the first node and returns it
 * 
 * @param q a pointer to non empty struct queue
 * 
 * @return first node in queue
*/
struct node queue_pop(struct queue *q);

#endif // QUEUE_H
