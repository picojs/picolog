CC     = gcc
CFLAGS = -std=c99 -Wall -pedantic -I ..
DEPS   = ../picolog.h

all: example

picolog.o: ../picolog.c $(DEPS)
	$(CC) -c -o picolog.o $< $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

example: example.o picolog.o $(DEPS)
	$(CC) -o example example.o picolog.o

.PHONY: clean

clean:
	rm example *.o
