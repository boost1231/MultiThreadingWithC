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
        action_input handler = action_queue_dequeue(input->action_queue);

        handler.action(handler.input);

        free(handler.input);
    }

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

    for (int i = 0; i < thread_pool_size; i++)
    {
        thread_pool_action_input_t *input = malloc(sizeof(thread_pool_action_input_t));

        input->my_id = i;

        input->action_queue = thread_pool->action_queue;

        pthread_create(&thread_pool->threads[i], NULL, thread_action, input);

        pthread_detach(thread_pool->threads[i]);
    }

    return 0;
}

void thread_pool_shutdown(thread_pool_t *thread_pool)
{
    // Some how I need to signal to all the threads to release from the queue and exit.
    // Do I have to JOIN on them?
    action_queue_destroy(thread_pool->action_queue);
}

void thread_pool_enqueue(thread_pool_t *thread_pool, action_input handler)
{
    action_queue_enqueue(thread_pool->action_queue, handler);
}

