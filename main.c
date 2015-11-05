#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

void print_stats(const char *label, const char *args, struct timespec tv0, size_t reps)
{
	FILE *f;
	char buf[256];
	struct timespec tv;
	int maj, min, in_heap=0;
	unsigned long l;
	size_t vm_size=0, vm_rss=0, vm_priv_dirty=0;

	clock_gettime(CLOCK_REALTIME, &tv);
	tv.tv_sec -= tv0.tv_sec;
	if ((tv.tv_nsec -= tv0.tv_nsec) < 0) {
		tv.tv_nsec += 1000000000;
		tv.tv_sec--;
	}

	f = fopen("/proc/self/smaps", "rb");
	if (f) while (fgets(buf, sizeof buf, f)) {
		if (sscanf(buf, "%*lx-%*lx %*s %*lx %x:%x %*lu %*s", &maj, &min)==2)
			in_heap = (!maj && !min && !strstr(buf, "---p") && (strstr(buf, "[heap]") || !strchr(buf, '[')));
		if (in_heap) {
			if (sscanf(buf, "Size: %lu", &l)==1) vm_size += l;
			else if (sscanf(buf, "Rss: %lu", &l)==1) vm_rss += l;
			else if (sscanf(buf, "Private_Dirty: %lu", &l)==1) vm_priv_dirty += l;
		}
	}
	if (f) fclose(f);
	printf("%s, %s, %s,  %ld.%.9ld, %zu, %E, %zu, %zu,  %zu\n",
    STRINGIFY(CC), label, args,
		(long)tv.tv_sec, (long)tv.tv_nsec,
    reps, reps/((double)tv.tv_sec + (double)tv.tv_nsec * 1e-9),
		vm_size, vm_rss, vm_priv_dirty);
}
void print_header(FILE *file)
{
  fprintf(file,"compiler, function, args, time, reps, rate, virt, res, dirty\n");
}
int run_bench(const char *label, const char *args, size_t (*bench)(void *), void *params)
{
	struct timespec tv0;
	pid_t p = fork();
	if (p) {
		int status;
		wait(&status);
		return status;
	}
	clock_gettime(CLOCK_REALTIME, &tv0);
	size_t reps = bench(params);
	print_stats(label, args, tv0,reps);
	exit(0);
}

#define RUN(a, b) \
	extern size_t (a)(void *); \
	run_bench(#a, "(" #b ")", (a), (b))

int main(int argc, char **argv)
{
#define ARGS\
  output(malloc,true)\
  output(environ,true)\
  output(string,false)\
  output(envdump,false)\
  output(regex,false)\
  output(utf8,false)\
  output(stdio,true)\
  output(pthread,true)
#define output(x,def) bool bench_##x = def;
  ARGS
#undef output
  struct arg_item{
    bool *value;
    const char *name;
  } ;
  struct arg_item items[] = {
#define output(x,def) { .value =  & bench_##x, .name = #x },
    ARGS
#undef output
  };
  for(int i = 1; i < argc; i ++ )
  {
    char *name = "";
    bool value = true;
    if(!strncmp(argv[i],"--no-",strlen("--no-")))
    {
      name = argv[i] + strlen("--no-");
      value = false;
    }
    else if (!strncmp(argv[i],"--",strlen("--")))
    {
      name = argv[i] + 2;
      value = true;
    }
    if ( name )
    {
      for(int j = 0; j < sizeof(items)/sizeof(items[0]); j++)
      {
        if(!strcmp(items[j].name,name))
        {
          *items[j].value = value;
          break;
        }
      }
    }
  }
  print_header(stdout);
  if(bench_malloc)
  {
    RUN(b_malloc_sparse, 0);
    RUN(b_malloc_bubble, 0);
    RUN(b_malloc_tiny1, 0);
    RUN(b_malloc_tiny2, 0);
    RUN(b_malloc_big1, 0);
    RUN(b_malloc_big2, 0);
    RUN(b_malloc_thread_stress, 0);
    RUN(b_malloc_thread_local, 0);
  }
  if(bench_environ)
  {
    RUN(b_environ_getenv,0);
    RUN(b_environ_putenv,0);
  }
  if(bench_envdump)
  {
    RUN(b_environ_dump_getenv,0);
    RUN(b_environ_dump_putenv,0);
  }
  if(bench_string)
  {
    RUN(b_string_strstr, "abcdefghijklmnopqrstuvwxyz");
    RUN(b_string_strstr, "azbycxdwevfugthsirjqkplomn");
    RUN(b_string_strstr, "aaaaaaaaaaaaaacccccccccccc");
    RUN(b_string_strstr, "aaaaaaaaaaaaaaaaaaaaaaaaac");
    RUN(b_string_strstr, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac");
    RUN(b_string_memset_tiny, 0);
    RUN(b_string_memset_small, 0);
    RUN(b_string_memset_medium, 0);
    RUN(b_string_memset_large, 0);
    RUN(b_string_memcpy_tiny, 0);
    RUN(b_string_memcpy_small, 0);
    RUN(b_string_memcpy_medium, 0);
    RUN(b_string_memcpy_large, 0);
    RUN(b_string_strchr, 0);
    RUN(b_string_strlen, 0);
  }
  if(bench_pthread)
  {
    RUN(b_pthread_createjoin_serial1, 0);
    RUN(b_pthread_createjoin_serial2, 0);
    RUN(b_pthread_create_serial1, 0);
    RUN(b_pthread_uselesslock, 0);
  }
  if(bench_utf8)
  {
  	RUN(b_utf8_bigbuf, 0);
	  RUN(b_utf8_onebyone, 0);
  }
  if(bench_stdio)
  {
    RUN(b_stdio_putcgetc, 0);
    RUN(b_stdio_putcgetc_unlocked, 0);
  }
  if(bench_regex)
  {
    RUN(b_regex_compile, "(a|b|c)*d*b");
  	RUN(b_regex_search, "(a|b|c)*d*b");
    RUN(b_regex_search, "a{25}b");
	}
}

