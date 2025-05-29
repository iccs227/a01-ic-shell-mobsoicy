CC=gcc
CFLAGS=-Wall -g 
BINARY=icsh
SRCS= icsh.c signals.c execute.c buildin.c

all: icsh

icsh: icsh.c
	$(CC) -o $(BINARY) $(CFLAGS) $(SRCS)

.PHONY: clean

clean:
	rm -f $(BINARY)
