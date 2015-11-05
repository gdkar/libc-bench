#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/mman.h>
#include <stdint.h>
#include <math.h>
#define BUFLEN 50000
extern char **environ;
static const char *printable = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-$#'!%*+,./\\\"{}|~";
static uint64_t x = (uint64_t) (M_PI * ULONG_MAX);
static inline uint64_t next()
{
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  return x * 2685821657736338717LL;
}
static char **b_environ_initenv()
{
  char **strings = calloc(sizeof(char*),1025);
  for(size_t  i = 0; i < 1024; i++)
  {
    size_t len = 0;
    while(len<2) len = next() & 0x3f;
    strings[i] = calloc(1,len+1);
    int c = printable[next() % strlen(printable)];
    memset(strings[i],c, len);
  }
  return strings;
}
static char **b_environ_init_putenv()
{
  char **strings = calloc(sizeof(char*),1025);
  for(size_t  i = 0; i < 1024; i++)
  {
    size_t len0 = 0;
    size_t len1 = 0;
    while(len0<2) len0 = next() & 0x3f;
    while(len1<2) len1 = next() & 0x3f;
    char *s = calloc(1,len0 + len1 + 2);
    int c = printable[next() % strlen(printable)];
    memset(s,c, len0);
    s[len0] = '=';
    c = printable[next() % strlen(printable)];
    memset(&s[len0+1], c, len1);
    strings[i] = s;
  }
  return strings;
}

static void b_environ_freeenv(char ** strings)
{
  char **ep = strings;
  while ( *ep )
  {
    free(*ep);
    ep++;
  }
  free(strings);
}
size_t b_environ_putenv(void *dummy)
{
  char **strings = b_environ_init_putenv();
  char **ep = strings;
  char *haystack;
  for(size_t i = 0; i < 1024*1024; i++)
  {
    putenv(strings[next() & 1023]);
  }
  clearenv();
  b_environ_freeenv(strings);
	return 1024 * 1024;
}
size_t b_environ_getenv(void *dummy)
{
  char **pstrings = b_environ_init_putenv();
  for(size_t i = 0; pstrings[i]; i ++ )
  {
    putenv(pstrings[i]);
  }
  char **strings = b_environ_initenv();
  char *haystack;
  for(size_t i = 0; i < 1024*1024; i++)
  {
    haystack = getenv(strings[next()&1023]);
  }
  clearenv();
  b_environ_freeenv(pstrings);
  b_environ_freeenv(strings );
	return 1024*1024;
}
size_t b_environ_dump_getenv(void *dummy)
{
  char **pstrings = b_environ_init_putenv();
  for(size_t i = 0; pstrings[i]; i ++ )
  {
    putenv(pstrings[i]);
  }
  char *haystack;
  fprintf(stderr,"DUMP GETENV\n");
  for(size_t i = 0; pstrings[i]; i++)
  {
    char *term = strchr(pstrings[i],'=');
    term = strndup(pstrings[i],term - pstrings[i] );
    fprintf(stderr,"%zu, %s = %s\n",i,term,getenv(term));
    free(term);
  }
  clearenv();
  b_environ_freeenv(pstrings);
  return 0;
}
size_t b_environ_dump_putenv(void *dummy)
{
  char **pstrings = b_environ_init_putenv();
  for(size_t i = 0; pstrings[i]; i ++ )
  {
    putenv(pstrings[i]);
  }
  char **ep = environ;
  fprintf(stderr,"DUMP PUTENV\n");
  for(size_t i = 0;*ep;ep++,i++)
  {
    fprintf(stderr,"%zu:\t%s\n",i,*ep);
  }

  clearenv();
  b_environ_freeenv(pstrings);
  return 0;
}
