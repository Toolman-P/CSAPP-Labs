#include "csapp.h"

#define MAXQUEUE 1024
typedef struct
{
  int buf[MAXQUEUE];
  int front, rear;
  sem_t mutex, slots, items;
} sbuf_t;

void init_sbuf(sbuf_t *sp);
int pop_sbuf(sbuf_t *sp);
void push_sbuf(sbuf_t *sp, int connfd);
