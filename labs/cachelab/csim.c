#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "cachelab.h"

#define SIZE_T_BITS (8 * sizeof(size_t))

typedef struct
{
  size_t valid : 1;
  size_t timestamp : 63;
  size_t tag;
} line_t;

line_t lines[32][8];
int s, E, b, t;

static line_t *find_lru(size_t set, int *evict);
static line_t *search(size_t set, size_t tag);
static void usage();
static void init_lines();
static void convert(size_t addr, size_t *set, size_t *tag);
static void update_timestamp(size_t set);
static void replace(line_t *line, size_t tag);

static void usage()
{
  printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
  printf("-h: Optional help flag that prints usage info\n");
  printf("-v: Optional verbose flag that displays trace info\n");
  printf("-s <s>:Number of set index bits (S = 2^s is the number of sets)\n");
  printf("-E <E>:Associativity (number of lines per set)\n");
  printf("-b <b>: Number of block bits (B = 2^b is the block size)\n");
  printf("-t <tracefile>: Name of the valgrind trace to replay\n");
}

static void init_lines()
{
  memset(lines, 0, sizeof(lines));
}

static line_t *find_lru(size_t set, int *evict)
{
  size_t timestamp = 0;
  int idx = 0;
  for (int i = 0; i < E; i++)
  {
    if (!lines[set][i].valid)
    {
      *evict = 0;
      return &lines[set][i];
    }
    if (lines[set][i].timestamp > timestamp)
    {
      timestamp = lines[set][i].timestamp;
      idx = i;
    }
  }
  *evict = 1;
  return &lines[set][idx];
}

static line_t *search(size_t set, size_t tag)
{
  for (int i = 0; i < E; i++)
  {
    if (lines[set][i].tag == tag && lines[set][i].valid)
    {
      lines[set][i].timestamp = 0;
      return &lines[set][i];
    }
  }
  return NULL;
}

static void convert(size_t addr, size_t *set, size_t *tag)
{
  *set = ((addr << t) >> (b + t));
  *tag = (addr >> (s + b));
}

static void update_timestamp(size_t set)
{
  for (int i = 0; i < E; i++)
    if (lines[set][i].valid)
      lines[set][i].timestamp++;
}

static void replace(line_t *line, size_t tag)
{
  line->valid = 1;
  line->timestamp = 0;
  line->tag = tag;
}

int main(int argc, char *argv[])
{
  FILE *file;
  int miss = 0, hit = 0, eviction = 0, verbose = 0, size, evict;
  char opt = '\0';
  size_t addr, set, tag;
  line_t *line;

  init_lines();

  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
  {
    switch (opt)
    {
    case 'h':
      usage();
      exit(0);
    case 'v':
      verbose = 1;
      break;
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      file = fopen(optarg, "r");
      break;
    default:
      usage();
      exit(1);
    }
  }

  if (!s || !E || !b || !file)
  {
    usage();
    exit(1);
  }
  else
  {
    t = SIZE_T_BITS - s - b;
  }

  do
  {
    fscanf(file, " ");
    switch (opt)
    {
    case 'I':
      break;
    case 'L':
    case 'S':
    case 'M':

      if (verbose)
        printf("%c %lx,%d ", opt, addr, size);

      convert(addr, &set, &tag);
      update_timestamp(set);

      if (search(set, tag))
      {
        hit++;
        if (verbose)
          printf("hit ");
      }
      else
      {
        line = find_lru(set, &evict);
        replace(line, tag);
        miss++;
        eviction += evict;
        if (verbose)
        {
          printf("miss ");
          if (evict)
            printf("eviction ");
        }
      }
      if (opt == 'M')
      {
        hit++;
        if (verbose)
          printf("hit ");
      }
      if (verbose)
        printf("\n");
      break;
    default:
      break;
    }
  } while (fscanf(file, "%c %lx,%d", &opt, &addr, &size) != EOF);

  printSummary(hit, miss, eviction);

  return 0;
}
