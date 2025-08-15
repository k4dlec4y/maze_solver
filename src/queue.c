#include <assert.h>

#include "queue.h"

void queue_init(queue_t *q)
{
    assert(q != NULL);

    q->first = NULL;
    q->last = NULL;
}

bool queue_is_empty(queue_t *q)
{
    assert(q != NULL);

    return q->first == NULL;
}

void queue_free(queue_t *q)
{
    assert(q != NULL);

    while (q->first != NULL) {
        node_t *next = q->first->next;
        free(q->first);
        q->first = next;
    }
    q->last = NULL;
}

bool queue_insert(queue_t *q, node_t insert_node)
{
    assert(q != NULL);

    node_t *new_node = malloc(sizeof *new_node);
    if (new_node == NULL)
        return false;

    *new_node = insert_node;
    new_node->next = NULL;
    if (q->first == NULL)
        q->first = new_node;

    if (q->last != NULL)
        q->last->next = new_node;

    q->last = new_node;
    return true;
}

node_t queue_pop(queue_t *q)
{
    assert(q != NULL);
    assert(q->first != NULL);

    node_t result = *(q->first);
    node_t *pop_node = q->first;
    if (q->first == q->last)
        q->last = NULL;

    q->first = q->first->next;
    free(pop_node);
    return result;
}
