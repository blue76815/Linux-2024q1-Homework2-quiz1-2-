CC = gcc
CFLAGS = -O0 -g -pthread -Wall

OBJS := main.o timsort.o

all: timtest

timtest: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

clean:
	rm -f $(OBJS)
	rm -f *.dSYM
	rm -f timtest
	rm -f .*.o.d