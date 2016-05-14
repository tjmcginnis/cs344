CC = gcc
CFLAGS=-Wall -g


all: shell
	chmod +x ./smallsh

shell: 	mcginnit.shell.c
	$(CC) $(CFLAGS) -o smallsh mcginnit.shell.c

clean:
	$(RM) smallsh
