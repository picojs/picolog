CC     = gcc
CFLAGS = -std=c99 -Wall -pedantic -I ..
DEPS   = ../picolog.h

all: example1 example2 example3

picolog.o: ../picolog.c $(DEPS)
	$(CC) -c -o picolog.o $< $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

example1: example1.o picolog.o $(DEPS)
	$(CC) -o example1 example1.o picolog.o

example2: example2.o picolog.o $(DEPS)
	$(CC) -o example2 example2.o picolog.o

example3: example3.o picolog.o $(DEPS)
	$(CC) -o example3 example3.o picolog.o

.PHONY: clean

clean:
	rm example1 example2 example3 *.o
