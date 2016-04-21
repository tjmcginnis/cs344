#include "dbg.h"    // REMOVE ME
#include <stdio.h>
#include <stdlib.h>


struct Room {
    char* room_name;
    struct Room *room_connections[6];
    char room_type;
};

struct Room *room_create(char* room_name, char room_type)
{
    int i;
    int arr_size;
    struct Room *new_room = malloc(sizeof(struct Room));
    new_room->room_name = room_name;
    new_room->room_type = room_type;

    // initialize room_connections to null

    return new_room;
}

void room_destroy(struct Room *room)
{
    free(room);
}

void add_connection(struct Room *room, struct Room *connection)
{
    // check that each room has less than 6 connections first
    // then add mutual connection between the two rooms
    printf("%i\n", sizeof(room));
    printf("%i\n", sizeof(room->room_connections));
}


int main(int argc, char *argv[])
{
    struct Room *first_room = room_create("First room", 's');
    struct Room *second_room = room_create("Second room", 'e');

    printf("%s\n", first_room->room_name);

    first_room->room_connections[0] = second_room;

    struct Room *room_ptr = first_room->room_connections[0];
    printf("%s\n", room_ptr->room_name);

    add_connection(first_room, second_room);

    room_destroy(first_room);
    room_destroy(second_room);

    return 0;
}
