
SRCS = $(sort $(wildcard *.c))
OBJS = $(SRCS:.c=.o)

CC ?= gcc
CFLAGS ?= -O3
CFLAGS += -g -ggdb
LDFLAGS = -static
LIBS += -lrt
CFLAGS += -DCC=${CC}

all: libc-bench

clean:
	rm -f $(OBJS) libc-bench

test: all
	./libc-bench

libc-bench: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
