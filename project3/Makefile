CC = gcc
CFLAGS=-Wall -g


all: shell tests
	chmod +x ./smallsh
	chmod +x ./tests

shell: 	builtins.o mcginnit.shell.c
	$(CC) $(CFLAGS) -o smallsh builtins.o mcginnit.shell.c

tests:	tests.c builtins.o
	$(CC) $(CFLAGS) -o tests builtins.o tests.c

builtins:	builtins.c builtins.h
	$(CC) $(CFLAGS) -c builtins.c


clean:
	$(RM) smallsh tests *.o
