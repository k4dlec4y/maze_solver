#include "queue.h"

#include <stdlib.h>
#include <assert.h>

void queue_init(struct queue *q)
{
    assert(q != NULL);
    q->first = NULL;
    q->last = NULL;
}

bool queue_is_empty(struct queue *q)
{
    assert(q != NULL);
    return q->first == NULL;
}

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

bool queue_insert(struct queue *q, struct node insert_node)
{
    assert(q != NULL);
    struct node *new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) {
        return false;
    }
    *new_node = insert_node;
    new_node->next = NULL;
    if (q->first == NULL) {
        q->first = new_node;
    }
    if (q->last != NULL) {
        q->last->next = new_node;
    }
    q->last = new_node;
    return true;
}

struct node queue_pop(struct queue *q)
{
    assert(q != NULL);
    assert(q->first != NULL);
    struct node result = *(q->first);
    struct node *pop_node = q->first;
    if (q->first == q->last) {
        q->last = NULL;
    }
    q->first = q->first->next;
    free(pop_node);
    return result;
}
