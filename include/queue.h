#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <unistd.h>

typedef struct node node_t;
struct node {
    /* used for freeing */
    node_t *next;
    /* index of node's vertex in vertices array */
    ssize_t index;
};

typedef
struct queue {
    node_t *first;
    node_t *last;
}
queue_t;

/**
 * Initializes the queue
 * 
 * @param q a pointer to queue_t
*/
void queue_init(queue_t *q);

/**
 * Checks whether the queue is empty
 * 
 * @param q a pointer to queue_t
 * 
 * @return true if queue is empty, false otherwise
*/
bool queue_is_empty(queue_t *q);

/**
 * Frees all nodes inside the queue
 * 
 * @param q a pointer to queue_t
*/
void queue_free(queue_t *q);

/**
 * Inserts a new node to the back of the queue
 * 
 * @param q a pointer to queue_t
 * @param insert_node node for insertion
 * 
 * @return true if insterted, false otherwise
*/
bool queue_insert(queue_t *q, node_t insert_node);

/**
 * Removes the first node and returns it
 * 
 * @param q a pointer to non empty queue_t
 * 
 * @return first node in queue
*/
node_t queue_pop(queue_t *q);

#endif  // QUEUE_H
