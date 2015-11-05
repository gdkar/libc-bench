#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

size_t b_malloc_sparse(void *dummy)
{
	void *p[1024*32];
	size_t i = 0, j = 0;
  for(j = 0; j < 32; j++)
  {
      for (i=0; i<sizeof p/sizeof *p; i++) {
        p[i] = malloc(4000);
        memset(p[i], 0, 4000);
      }
      for (i=0; i<sizeof p/sizeof *p; i++)
        if (i%150) free(p[i]);
  }
	return i * j;
}
size_t b_malloc_bubble(void *dummy)
{
	void *p[1024*16];
	size_t i = 0, j = 0;
  for(; j < 32; j ++)
  {
    for (i=0; i<sizeof p/sizeof *p; i++) {
      p[i] = malloc(4000);
      memset(p[i], 0, 4000);
    }
    for (i=0; i<sizeof p/sizeof *p-1; i++)
      free(p[i]);
  }
	return i * j;
}

size_t b_malloc_tiny1(void *dummy)
{
	void *p[1024*16];
	size_t i;
	for (i=0; i<sizeof p/sizeof *p; i++) {
		p[i] = malloc((i%4+1)*16);
	}
	for (i=0; i<sizeof p/sizeof *p; i++) {
		free(p[i]);
	}
	return i;
}

size_t b_malloc_tiny2(void *dummy)
{
	void *p[1024 * 16];
	size_t i = 0, j = 0;
  for(;j < 32; j++)
  {
    for (i=0; i<sizeof p/sizeof *p; i++) {
      p[i] = malloc((i%4+1)*16);
    }
    for (i=1; i; i = (i+57)%(sizeof p/sizeof *p))
      free(p[i]);
  }
	return j * sizeof p/sizeof *p;
}

size_t b_malloc_big1(void *dummy)
{
	void *p[2048];
	size_t i = 0, j = 0;
  for(;j < 32; j++)
  {
    for (i=0; i<sizeof p/sizeof *p; i++) {
      p[i] = malloc((i%4+1)*16384);
    }
    for (i=0; i<sizeof p/sizeof *p; i++) {
      free(p[i]);
    }
  }
	return i * j;
}

size_t b_malloc_big2(void *dummy)
{
	void *p[2048];
	size_t i = 0, j = 0;
  for(;j<32;j++)
  {
    for (i=0; i<sizeof p/sizeof *p; i++) {
      p[i] = malloc((i%4+1)*16384);
    }
    for (i=1; i; i = (i+57)%(sizeof p/sizeof *p))
      free(p[i]);
  }
	return j * sizeof p/sizeof *p;
}


#define LOOPS (1<<16)
#define SH_COUNT 64
#define MAX_SZ 512

struct foo {
	pthread_mutex_t lock;
	_Atomic(void*)mem;
};

static unsigned rng(unsigned *r)
{
	return *r = *r * 1103515245 + 12345;
}


static void *stress(void *arg)
{
	struct foo *foo = arg;
	unsigned r = (unsigned)pthread_self();
	int i, j;
	size_t sz;
	void *p;

	for (i=0; i<LOOPS; i++)
  {
		j = rng(&r)  % SH_COUNT;
		sz = rng(&r) % MAX_SZ;
		p = atomic_exchange(&foo[j].mem,0);
		free(p);
		if (!p)
    {
			p = malloc(sz);
      p = atomic_exchange(&foo[j].mem,p);
			free(p);
		}
	}
	return (void*)i;
}

size_t b_malloc_thread_stress(void *dummy)
{
	struct foo foo[SH_COUNT] = {0};
	pthread_t tid[64];
  size_t res = 0;
	int i;
  for(i = 0; i < sizeof(tid)/sizeof(*tid);i++)
  {
    pthread_create(&tid[i],NULL,stress,foo);  
  }
  for(i = 0; i < sizeof(tid)/sizeof(*tid);i++)
  {
    void *_res;
    pthread_join(tid[i],&_res);
    res += (size_t)_res;
  }
	return res;
}

size_t b_malloc_thread_local(void *dummy)
{
	struct foo foo[SH_COUNT][64] = {0};
	pthread_t tid[64];
	size_t res = 0;
	int i;
  for(i = 0; i < sizeof(tid)/sizeof(*tid);i++)
  {
    pthread_create(&tid[i],NULL,stress,foo[i]);  
  }
  for(i = 0; i < sizeof(tid)/sizeof(*tid);i++)
  {
    void *_res;
    pthread_join(tid[i],&_res);
    res += (size_t)_res;
  }
	return res;
}
