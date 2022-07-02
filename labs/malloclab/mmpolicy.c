#include <unistd.h>
#include <memory.h>

#include "mmstruct.h"
#include "mmpolicy.h"
#include "memlib.h"

static free_t *freeblocks[MAXLIST] = {NULL};
static alloc_t *allocblocks[MAXLIST] = {NULL};

static size_t convert(size_t s);
static void insert(link_t *block, size_t idx, int mode);
static free_t *searchfree(size_t idx, size_t s);
static alloc_t *searchallocated(void *ptr, size_t idx);
static free_t *init_block(void *ptr, size_t s);
static free_t *split_block(free_t *prev, size_t sub);

static size_t convert(size_t s)
{
  s /= ALIGNMENT;
  static size_t m = 0;
  if ((m = s / MAXPART))
  {
    return MAXPART + m;
  }
  else
  {
    return s;
  }
}

static free_t *init_block(void *ptr, size_t s)
{
  free_t *block = mem_sbrk(sizeof(free_t));
  if ((void *)block == (void *)(-1))
    return block;
  block->start = ptr;
  block->size = s;
  block->next = NULL;
  return block;
}

static free_t *split_block(free_t *prev, size_t sub)
{
  prev->size = prev->size - sub;
  insert(prev, convert(prev->size), FREE);
  return init_block(INCR(prev->start, sub), sub);
}

static link_t *searchfree(size_t idx, size_t s)
{
  free_t *prev, *cur;
  if ((prev = freeblocks[idx]) == NULL)
    return NULL;
  if ((cur = prev->next) == NULL && prev->size >= s)
  {
    freeblocks[idx] = NULL;
    return prev;
  }
  while (cur)
  {
    if (cur->size >= s)
    {
      prev->next = cur->next->next;
      return cur;
    }
    prev = cur;
    cur = cur->next;
  }
  return NULL;
}

static alloc_t *searchallocated(void *ptr, size_t idx)
{
  free_t *prev, *cur;
  if ((prev = freeblocks[idx]) == NULL)
    return NULL;
  if ((cur = prev->next) == NULL && prev->start == ptr)
  {
    freeblocks[idx] = NULL;
    return prev;
  }
  while (cur)
  {
    if (cur->start == ptr)
    {
      prev->next = cur->next->next;
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

static void insert(link_t *block, size_t idx, int mode)
{
  switch (mode)
  {
  case FREE:
    block->next = freeblocks[idx];
    freeblocks[idx] = block;
    break;
  case ALLOC:
    block->next = allocblocks[idx];
    allocblocks[idx] = block;
    break;
  }
}

void *alloc_block(size_t s)
{
  free_t *fp = NULL;
  size_t idx = convert(s);
  for (size_t i = idx; i <= MAXLIST; i++)
  {
    if ((fp = searchfree(i, s)))
    {
      if (fp->size == s)
      {
        break;
      }
      else
      {
        free_t *lp = split_block(fp, s);
        fp = lp;
        break;
      }
    }
  }
  if (!fp)
    fp = init_block(mem_sbrk(s), s);
  if ((void *)fp == (void *)-1)
    return fp;
  insert(fp, idx, ALLOC);
  return fp->start;
}

int free_block(void *ptr)
{
  for (size_t i = 1; i <= MAXLIST; i++)
  {
    alloc_t *ap = searchallocated(ptr, i);
    if (ap)
    {
      insert(ap, i, FREE);
      return 1;
    }
  }
  return 0;
}

void *realloc_block(void *ptr, size_t size)
{
  alloc_t *ap;
  free_t *fp;
  for (size_t i = 1; i <= MAXLIST; i++)
  {
    ap = searchallocated(ptr, i);
    if (ap)
    {
      if (ap->size > size)
      {
        fp = split_block(ap, size);
        insert(fp, convert(size), ALLOC);
        return fp->start;
      }
      else if (ap->size == size)
      {
        return ptr;
      }
    }
  }
  fp = init_block(mem_sbrk(size), size);
  if ((void *)fp == (void *)-1)
    return fp;
  memcpy(ptr, fp->start, size);
  return fp->start;
}
