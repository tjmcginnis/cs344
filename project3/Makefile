CC = gcc
CFLAGS=-Wall -g

default: shell tests

all:
	chmod +x ./shell
	chmod +x ./tests

shell: 		builtins.o mcginnit.shell.c
	$(CC) $(CFLAGS) -o shell builtins.o mcginnit.shell.c

tests:		tests.c builtins.o
	$(CC) $(CFLAGS) -o tests builtins.o tests.c

builtins:	builtins.c builtins.h
	$(CC) $(CFLAGS) -c builtins.c


clean:
	# rm -f shell
	$(RM) shell tests *.o
