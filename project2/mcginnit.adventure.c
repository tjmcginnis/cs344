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

int count_connections(struct Room *room)
{
    int i;
    int count = 0;

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (room->room_connections[i] != NULL) {
            count += 1;
        }
    }
    return count;
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

void add_connection(struct Room *room, struct Room *connection)
{
    // check that each room has less than 6 connections first
    int room_one_count = count_connections(room);
    int room_two_count = count_connections(connection);
    // check that rooms aren't already connected
    int connected = check_connection(room, connection);
    // then add mutual connection between the two rooms
    if (room_one_count < 6 && room_two_count < 6 && connected == 1) {
        room->room_connections[room_one_count] = connection;
        connection->room_connections[room_two_count] = room;
    }
}



int main(int argc, char *argv[])
{
    struct Room *first_room = room_create("First room", 's');
    struct Room *second_room = room_create("Second room", 'e');

    printf("%s\n", first_room->room_name);

    add_connection(first_room, second_room);

    room_destroy(first_room);
    room_destroy(second_room);

    return 0;
}
