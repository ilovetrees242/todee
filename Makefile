CC = gcc
LDFLAGS = -lncurses

all: todee.c
	$(CC) $(LDFLAGS) todee.c -o todee
