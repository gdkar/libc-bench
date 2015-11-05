#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#define BUFLEN 50000

size_t b_string_strstr(void *dummy)
{
	const char *needle = dummy;
	size_t l = strlen(needle);
	size_t i;
	size_t cnt = 10000;
	size_t cs = 0;
	char *haystack = malloc(l * cnt + 1);
	for (i=0; i<cnt-1; i++) {
		memcpy(haystack + l*i, needle, l);
		haystack[l*i+l-1] ^= 1;
	}
	memcpy(haystack + l*i, needle, l+1);
	for (i=0; i<50; i++) {
		haystack[0]^=1;
		cs += !strstr(haystack, needle);
	}
	free(haystack);
	return i;
}

size_t b_string_memset_tiny(void *dummy)
{
#define LEN 32
	char *buf = malloc(LEN*2);
	size_t i;
	for (i=0; i<(1<<28); i++)
		memset(buf+(i&(LEN-1)), i, LEN);
	free(buf);
	return i;
#undef LEN
}
size_t b_string_memset_small(void *dummy)
{
#define LEN (1<<10)
	char *buf = malloc(LEN*2);
	size_t i;
	for (i=0; i<(1<<23); i++)
		memset(buf+(i&(LEN-1)), i, LEN);
	free(buf);
	return i;
#undef LEN
}
size_t b_string_memset_medium(void *dummy)
{
#define LEN (1<<14)
	char *buf = malloc(LEN*2);
	size_t i;
	for (i=0; i<(1<<19); i++)
		memset(buf+(i&(LEN-1)), i, LEN);
	free(buf);
	return i;
#undef LEN
}
size_t b_string_memset_large(void *dummy)
{
#define LEN (1<<20)
	char *buf = malloc(LEN*2);
	size_t i;
	for (i=0; i<(1<<13); i++)
		memset(buf+i, i, LEN);
	free(buf);
	return i;
#undef LEN
}
size_t b_string_memcpy_tiny(void *dummy)
{
	char *src = malloc(32);
  char *dst = malloc(32);
	size_t i;
	for (i=0; i<(1<<26); i++)
  {
    memcpy(dst,src + (i&16),16 + i & 16);
  }
	free(dst);
  free(src);
	return i;
}
size_t b_string_memcpy_small(void *dummy)
{
	char *src = malloc(1024);
  char *dst = malloc(1024);
	size_t i;
  for(i = 0; i < (1<<1024); i++)
    src[i] = i&255;

	for (i=0; i<(1<<22); i++)
  {
    memcpy(dst,src,(i & 511)+512);
  }
	free(dst);
  free(src);
	return i;
}
size_t b_string_memcpy_medium(void *dummy)
{
	char *src = malloc(16384);
  char *dst = malloc(16384);
	size_t i;
  for(i = 0; i < 16384; i++)
    src[i] = i&255;

	for (i=0; i<(1<<18); i++)
  {
    memcpy(dst,src + (i&8192),16384 -( i &8191));
  }
	free(dst);
  free(src);
	return i;
}
size_t b_string_memcpy_large(void *dummy)
{
	char *src = malloc(1<<20);
	char *dst = malloc(1<<20);
	size_t i;
  for(i = 0; i < (1<<20); i++)
    src[i] = i;
	for (i=0; i<(1<<14); i++)
  {
    memcpy(dst,src + (i&((1<<19)-1)), 1<<19);
  }
  free(src);
  free(dst);
	return i;
}
size_t b_string_strchr(void *dummy)
{
	char *buf = malloc(BUFLEN);
	size_t i;
	size_t cs;
	memset(buf, 'a', BUFLEN);
	buf[BUFLEN-1] = 0;
	buf[BUFLEN-2] = 'b';
	for (i=0; i<100; i++) {
		buf[i] = '0'+i%8;
		cs += (int)strchr(buf, 'b');
	}
	free(buf);
	return i;
}

size_t b_string_strlen(void *dummy)
{
	char *buf = malloc(BUFLEN);
	size_t i;
	size_t cs = 0;

	memset(buf, 'a', BUFLEN-1);
	buf[BUFLEN-1] = 0;
	for (i=0; i<1024*1024; i++) {
		buf[i] = '0'+i%8;
		cs += strlen(buf);
	}
	free(buf);
	return i;
}
