#include <stddef.h>

#define MAXPART 512
#define MAXLIST 2 * MAXPART + 1
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)
#define INCR(ptr, s) ((size_t *)ptr + s)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define FREE 0
#define ALLOC 1

void *alloc_block(size_t size);
int free_block(void *ptr);
void *realloc_block(void *ptr, size_t size);
