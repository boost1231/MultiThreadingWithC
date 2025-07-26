#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread_pool.h"


#define MY_THREAD_POOL_MAX_SIZE 20

static const uint32_t MAX_SIZE = MY_THREAD_POOL_MAX_SIZE;

typedef struct {
    int my_id;
    action_queue_t *action_queue;
} thread_pool_action_input_t;

static void *thread_action(void *arg)
{
    thread_pool_action_input_t *input = arg;

    printf("Starting a thread. %d\n", input->my_id);

    // We need a way to send a signal to cause the loop to exit.
    while (1) {
        action_item_t handler = action_queue_dequeue(input->action_queue);

        if (handler.queue_closed) {
            break;
        }

        handler.action(handler.input);

        free(handler.input);
    }

    printf("Exiting thread %d.\n", input->my_id);

    return NULL;
}

size_t thread_pool_thread_pool_t_size(int thread_pool_size)
{
    if (thread_pool_size > MAX_SIZE)
    {
        return -1;
    }

    return sizeof(thread_pool_t) + thread_pool_size*sizeof(pthread_t);
}

int thread_pool_create(thread_pool_t *thread_pool, int thread_pool_size, int queue_size)
{
    thread_pool->action_queue = action_queue_init(queue_size);
    thread_pool->thread_count = thread_pool_size;

    for (int i = 0; i < thread_pool_size; i++)
    {
        thread_pool_action_input_t *input = malloc(sizeof(thread_pool_action_input_t));

        input->my_id = i;

        input->action_queue = thread_pool->action_queue;

        pthread_create(&thread_pool->threads[i], NULL, thread_action, input);
    }

    return 0;
}

void thread_pool_shutdown(thread_pool_t *thread_pool)
{
    // When 'action_queue_drain returns, I know there
    // is nothing on the queue and it's safe to destroy it.
    action_queue_drain(thread_pool->action_queue);

    printf("Waiting for thread pool threads to exit.\n");

    for (int i = 0; i < thread_pool->thread_count; i++) {
        // May want to handle error conditions
        pthread_join(thread_pool->threads[i], NULL);
        printf("Joined to thread %d.\n", i);
    }

    action_queue_destroy(thread_pool->action_queue);
}

void thread_pool_enqueue(thread_pool_t *thread_pool, action_input handler)
{
    action_queue_enqueue(thread_pool->action_queue, handler);
}

