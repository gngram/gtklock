// gtklock
// Copyright (c) 2025 Ganga Ram

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <gtk/gtk.h>

#define MSG_SUCCESS  0
#define MSG_TERM  1
#define MSG_ERROR  2
#define MSG_INFO  3
#define MSG_DEBUG  4

struct Message {
    char *data;
    unsigned int type;
};

typedef gpointer MessageQ;

MessageQ init_message_q(int max_size);
void enqueue(MessageQ mq, struct Message *msg);
struct Message* dequeue(MessageQ mq);
void free_message_q(MessageQ mq);