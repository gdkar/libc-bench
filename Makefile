SRCS = $(sort $(wildcard *.c))
OBJS = $(SRCS:.c=.o)

CC ?= gcc
CFLAGS ?= -O3
CFLAGS += -g -ggdb
LDFLAGS = -static
LIBS += -lrt
CFLAGS += -DCC=${CC}

all: bin/libc-bench

clean:
	rm -f $(OBJS) bin/libc-bench

test: all
	./bin/libc-bench

bin/libc-bench: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
