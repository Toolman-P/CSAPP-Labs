#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_WORKERS 8
/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

typedef struct sockaddr sa_t;
typedef struct sockaddr_storage sas_t;
typedef struct
{
    char hostname[MAXLINE];
    char port[MAXLINE];
    char uriargs[MAXLINE];
} url_t;

typedef struct
{
    char method[10];
    char url[MAXLINE];
    char version[10];
} http_t;

typedef struct
{
    char buf[MAX_OBJECT_SIZE];
    rio_t sio, cio;
} worker_t;

typedef struct
{
    url_t url;
    char buf[MAX_OBJECT_SIZE];
    size_t alloc;
} cache_t;

#define MAX_CACHES ((MAX_CACHE_SIZE) / sizeof(cache_t))

static void *thread(void *vargsp);
static void init_worker_pool();
static void free_worker(worker_t *worker);

static void response(int connfd, worker_t *worker);
static int receive(int connfd, char *buf, rio_t *cio);
static int forward_request(url_t *url, char *buf, rio_t *cio, rio_t *sio);
static int forward_cache(url_t *url, rio_t *cio);
static int parse_uri(url_t *url, char *buf);
static int urlcmp(url_t *url1, url_t *url2);
static void urlcopy(url_t *dest, url_t *src);
static void free_url(url_t *url);

static void init_caches(cache_t *caches);
static void free_cache(cache_t *cache);
static void fill_cache(cache_t *cache, url_t *url, char *buf, int alloc);
