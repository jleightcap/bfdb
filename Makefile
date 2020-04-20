BIN := bfdb
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

CFLAGS := -O2 -Wall -Wextra -pedantic -std=c99

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c $(wildcard .h)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN) $(OBJS) dump.dat
