#include "message_q.h"

typedef struct {
    struct Message **buffer;  // Array to hold messages
    int head;                 // Index of the first item
    int tail;                 // Index of the next available spot for insertion
    int size;                 // Current number of elements in the queue
    int max_size;             // Maximum size of the queue
    pthread_mutex_t mutex;
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;
} MessageQueue;

MessageQ init_message_q(int max_size)
{
    MessageQueue *queue = malloc(sizeof(MessageQueue));
    if (queue == NULL) {
        perror("Allocation failed. No memory left on device!");
        return NULL;
    }

    queue->buffer = malloc(sizeof(struct Message*) * max_size);  // Allocate space for messages
    if (queue->buffer == NULL) {
        perror("Buffer allocation failed. No memory left on device!");
        free(queue);
        return NULL;
    }

    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
    queue->max_size = max_size;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_full, NULL);
    pthread_cond_init(&queue->cond_empty, NULL);

    return (MessageQ)queue;
}

void enqueue(MessageQ mq, struct Message *msg)
{
    MessageQueue *queue = (MessageQueue*)mq;
    pthread_mutex_lock(&queue->mutex);

    // Wait until there is space in the queue
    while (queue->size == queue->max_size) {
        pthread_cond_wait(&queue->cond_full, &queue->mutex);
    }

    // Insert message into the buffer
    queue->buffer[queue->tail] = msg;
    queue->tail = (queue->tail + 1) % queue->max_size;  // Wrap-around

    queue->size++;

    // Signal any waiting dequeue operation that there is now data
    pthread_cond_signal(&queue->cond_empty);

    pthread_mutex_unlock(&queue->mutex);
}

struct Message* dequeue(MessageQ mq)
{
    MessageQueue *queue = (MessageQueue*)mq;
    pthread_mutex_lock(&queue->mutex);

    // Wait until the queue is not empty
    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond_empty, &queue->mutex);
    }

    // Get message from the buffer
    struct Message *msg = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % queue->max_size;  // Wrap-around

    queue->size--;

    // Signal any waiting enqueue operation that space has become available
    pthread_cond_signal(&queue->cond_full);

    pthread_mutex_unlock(&queue->mutex);

    return msg;
}

void free_message_q(MessageQ mq)
{
    MessageQueue *queue = (MessageQueue*)mq;

    // Free all messages in the queue
    for (int i = 0; i < queue->size; ++i) {
        int index = (queue->head + i) % queue->max_size;
        free(queue->buffer[index]->data);
        free(queue->buffer[index]);
    }

    free(queue->buffer);
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_full);
    pthread_cond_destroy(&queue->cond_empty);
    free(queue);
}
