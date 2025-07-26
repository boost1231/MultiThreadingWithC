#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "action_queue.h"

/*
 * ToDo:
 * - Handle errno from all these method calls
 */

action_queue_t *action_queue_init(uint32_t max_queue_size)
{
    size_t full_size = sizeof(action_queue_t) + max_queue_size*sizeof(void (*)(void *));

    action_queue_t *queue = malloc(full_size);

    memset(queue, 0, full_size);

    queue->max_queue_size = max_queue_size;
    queue->head_index = 0;
    queue->tail_index = 0;
    queue->queued_items_count = 0;
    pthread_cond_init(&queue->queue_condition, NULL);
    pthread_cond_init(&queue->dequeue_condition, NULL);
    pthread_cond_init(&queue->shutdown_condition, NULL);
    pthread_mutex_init(&queue->lock, NULL);

    return queue;
}

void action_queue_drain(action_queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    queue->queue_closed = 1;

    while (queue->queued_items_count > 0) {
        pthread_cond_wait(&queue->shutdown_condition, &queue->lock);
    }

    assert(queue->queued_items_count == 0);

    pthread_cond_broadcast(&queue->dequeue_condition);
    // When this method returns, then we know we are good to call destroy.

    pthread_mutex_unlock(&queue->lock);
}

void action_queue_destroy(action_queue_t *queue)
{
    pthread_cond_destroy(&queue->queue_condition);
    pthread_cond_destroy(&queue->dequeue_condition);
    pthread_cond_destroy(&queue->shutdown_condition);
    pthread_mutex_destroy(&queue->lock);
    free(queue);
}

static int increment_index(uint32_t index, uint32_t max_queue_size)
{
    if (index == max_queue_size - 1) {
        return 0;
    }
    else {
        return ++index;
    }
}

void action_queue_enqueue(action_queue_t *queue, action_input handler)
{
    pthread_mutex_lock(&queue->lock);

    assert(queue->queued_items_count <= queue->max_queue_size);

    while (queue->queued_items_count == queue->max_queue_size && !queue->queue_closed)
    {
        pthread_cond_wait(&queue->queue_condition, &queue->lock);
        assert(queue->queued_items_count <= queue->max_queue_size);
    }

    if (!queue->queue_closed) {
        queue->handlers[queue->tail_index] = handler;

        queue->queued_items_count++;
        queue->tail_index = increment_index(queue->tail_index, queue->max_queue_size);

        pthread_cond_signal(&queue->dequeue_condition);
    }

    // We should probably some how return an error to the caller if the queue
    // has been closed so they know not to keep queueing things.

    pthread_mutex_unlock(&queue->lock);
}

action_item_t action_queue_dequeue(action_queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);

    assert(queue->queued_items_count <= queue->max_queue_size);

    while (queue->queued_items_count == 0 && !queue->queue_closed) {
        pthread_cond_wait(&queue->dequeue_condition, &queue->lock);
        assert(queue->queued_items_count <= queue->max_queue_size);
    }

    action_item_t action_to_run;

    if (queue->queue_closed && queue->queued_items_count == 0) {
        action_to_run.queue_closed = 1;
        pthread_cond_signal(&queue->shutdown_condition);
    }
    else {
        action_input action = queue->handlers[queue->head_index];

        queue->queued_items_count--;

        queue->head_index = increment_index(queue->head_index, queue->max_queue_size);

        action_to_run.queue_closed = 0;
        action_to_run.action = action.action;
        action_to_run.input = action.input;

        pthread_cond_signal(&queue->queue_condition);
    }

    pthread_mutex_unlock(&queue->lock);

    return action_to_run;
}
