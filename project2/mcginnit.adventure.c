#include "dbg.h"    // REMOVE ME
#include <stdio.h>
#include <stdlib.h>


#define MAX_CONNECTIONS 6

struct Room {
    char* room_name;
    struct Room *room_connections[MAX_CONNECTIONS];
    char room_type;
};

struct Room *room_create(char* room_name, char room_type)
{
    int i;
    struct Room *new_room = malloc(sizeof(struct Room));
    new_room->room_name = room_name;
    new_room->room_type = room_type;

    // initialize room_connections to null
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        new_room->room_connections[i] = NULL;
    }

    return new_room;
}

void room_destroy(struct Room *room)
{
    free(room);
}

void add_connection(struct Room *room, struct Room *connection)
{
    // check that each room has less than 6 connections first
    // check that rooms aren't already connected
    // then add mutual connection between the two rooms
    printf("%i\n", sizeof(room));
    printf("%i\n", sizeof(room->room_connections));
}

int check_connection(struct Room *room, struct Room *connection)
{
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (room->room_connections[i] == connection) {
            return 0;
        }
    }
    return 1;
}


int main(int argc, char *argv[])
{
    struct Room *first_room = room_create("First room", 's');
    struct Room *second_room = room_create("Second room", 'e');

    printf("%s\n", first_room->room_name);

    first_room->room_connections[0] = second_room;

    struct Room *room_ptr = first_room->room_connections[0];
    printf("%s\n", room_ptr->room_name);

    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        printf("%p\n", first_room->room_connections[i]);
    }
    int connected = check_connection(first_room, second_room);
    printf("%i\n", connected);

    room_destroy(first_room);
    room_destroy(second_room);

    return 0;
}
