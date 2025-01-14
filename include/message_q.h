// gtklock
// Copyright (c) 2025 Ganga Ram

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MSG_ERROR = 0;
#define MSG_INFO = 1;

typedef struct {
  char *data;
  unsigned int type;
} Message;

typedef void* MessageQ;

MessageQ init_mq(void);
void enqueue(MessageQ mq, const Message *msg);
const Message* dequeue(MessageQ *mq);
void free_mq(MessageQ *mq);