SRCS = $(sort $(wildcard *.c))
OBJS = $(SRCS:.c=.o)

CFLAGS ?= -O3
CFLAGS += -g -ggdb -D__CC__=${CC}
LDFLAGS = -static
LIBS += -lrt

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
