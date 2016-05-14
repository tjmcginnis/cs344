CC = gcc
CFLAGS=-Wall -g


all: shell
	chmod +x ./smallsh

shell: 	builtins.o mcginnit.shell.c
	$(CC) $(CFLAGS) -o smallsh mcginnit.shell.c

clean:
	$(RM) smallsh
