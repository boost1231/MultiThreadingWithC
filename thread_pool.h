#include "action_queue.h"

typedef struct {
    action_queue_t *action_queue;
    pthread_t threads[];
} thread_pool_t;


size_t thread_pool_thread_pool_t_size(int thread_pool_size);

int thread_pool_create(thread_pool_t *thread_pool, int thread_pool_size, int queue_size);

void thread_pool_shutdown(thread_pool_t * thread_pool);

void thread_pool_enqueue(thread_pool_t *thread_pool, action_input handler);
