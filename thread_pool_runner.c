#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread_pool.h"

void get_test_action(void *letter_input)
{
    char *letter = (char *)letter_input;
    printf("Going To Sleep: %c\n", *letter);
    sleep(3);
    printf("Woke Up: %c\n", *letter);
}

int main(int argc, char **argv)
{
    const int thread_pool_thread_count = 5;
    const int thread_pool_queue_max_queue_item_count = 3;

    size_t thread_pool_size = thread_pool_thread_pool_t_size(thread_pool_thread_count);

    thread_pool_t *thread_pool = malloc(thread_pool_size);

    memset(thread_pool, 0, thread_pool_size);

    thread_pool_create(thread_pool, thread_pool_thread_count, thread_pool_queue_max_queue_item_count);

    char x = 'a';

    while ((x = getchar()) != 'q') {

        if (x == '\n') {
          continue;
        }

        char *y = malloc(sizeof(char));
        *y = x;

        action_input input;
        input.action = get_test_action;
        input.input = y;

        thread_pool_enqueue(thread_pool, input);
    }

    thread_pool_shutdown(thread_pool);

    printf("The Program is Over!!!!\n");
}

