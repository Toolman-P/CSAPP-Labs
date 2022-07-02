#ifndef MMSTRUCT_H
#define MMSTRUCT_H

#include <stddef.h>
typedef struct linklist
{
  size_t size;
  struct linklist *next;
  void *start;
} link_t;

typedef link_t free_t;
typedef link_t alloc_t;
#endif
