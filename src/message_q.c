#include "message_q.h"

typedef struct Node {
    Message *message;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;
} MessageQueue;

MessageQ init_mq(void)
{
    MessageQueue *queue = malloc(sizeof(MessageQueue));
    if (queue == NULL) {
        g_warning("Allocation failed. No memory left on device!");
        return NULL;
    }

    queue->front = NULL;
    queue->rear = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_full, NULL);
    pthread_cond_init(&queue->cond_empty, NULL);

    return (MessageQ)queue;
}

void enqueue(MessageQ mq, const Message *msg)
{
    MessageQueue *queue = (MessageQueue*)mq;
    pthread_mutex_lock(&queue->mutex);

    Node *msgnode = (Node *)malloc(sizeof(Node));
    msgnode->message = msg;
    msgnode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = msgnode;
        queue->rear = msgnode;
    } else {
        queue->rear->next = msgnode;
        queue->rear = msgnode;
    }

    pthread_cond_signal(&queue->cond_empty);
    pthread_mutex_unlock(&queue->mutex);
}

const Message* dequeue(void *mq)
{
    MessageQueue *queue = (MessageQueue*)mq;
    pthread_mutex_lock(&queue->mutex);

    while (queue->front == NULL) {
        pthread_cond_wait(&queue->cond_empty, &queue->mutex);
    }

    Node *temp = queue->front;
    Message *msg = temp->message;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);

    pthread_cond_signal(&queue->cond_full);
    pthread_mutex_unlock(&queue->mutex);

    return msg;
}

void free_mq(void *mq)
{
    MessageQueue *queue = (MessageQueue*)mq;
    while (queue->front != NULL) {
        Node *temp = queue->front;
        queue->front = queue->front->next;
        free(temp->message->data);
        free(temp->message);
        free(temp);
    }

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_full);
    pthread_cond_destroy(&queue->cond_empty);
    free(mq);
}
