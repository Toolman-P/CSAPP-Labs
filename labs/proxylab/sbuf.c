#include "sbuf.h"

void init_sbuf(sbuf_t *sp)
{
  memset(sp->buf, 0, sizeof(sp->buf));
  sp->front = sp->rear = 0;
  Sem_init(&sp->mutex, 0, 1);
  Sem_init(&sp->items, 0, 0);
  Sem_init(&sp->slots, 0, MAXQUEUE);
}

int pop_sbuf(sbuf_t *sp)
{
  int item;
  P(&sp->items);
  P(&sp->mutex);
  item = sp->buf[sp->front];
  sp->front = (sp->front + 1) % MAXQUEUE;
  V(&sp->mutex);
  V(&sp->slots);
  return item;
}

void push_sbuf(sbuf_t *sp, int connfd)
{
  int rear;
  P(&sp->slots);
  P(&sp->mutex);
  rear = sp->rear;
  sp->buf[rear] = connfd;
  rear = (rear + 1) % MAXQUEUE;
  sp->rear = rear;
  V(&sp->mutex);
  V(&sp->items);
}
