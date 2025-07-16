#include <stdint.h>
#include <pthread.h>

typedef void (*action_queue_action)(void *);

typedef struct {
    action_queue_action action;
    void * input;
} action_input;

typedef struct {
  uint32_t max_queue_size;
  uint32_t array_size;
  uint32_t head_index;
  uint32_t tail_index;
  uint32_t queued_items_count;
  pthread_cond_t queue_condition;
  pthread_cond_t dequeue_condition;
  pthread_mutex_t lock;
  action_input handlers[];
} action_queue_t;


action_queue_t *action_queue_init(uint32_t max_queue_size);

void action_queue_destroy(action_queue_t *queue);

void action_queue_enqueue(action_queue_t *queue, action_input handler);

action_input action_queue_dequeue(action_queue_t *queue);
