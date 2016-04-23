#include "dbg.h"    // REMOVE ME
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define MAX_CONNECTIONS 6

const char* ROOM_NAMES[] = {};

struct Room {
    char* room_name;
    struct Room *room_connections[MAX_CONNECTIONS];
    char* room_type;
};

struct Room *room_create(char* room_name, char* room_type)
{
    int i;
    struct Room *new_room = malloc(sizeof(struct Room));

    assert(new_room != NULL);

    new_room->room_name = room_name;
    new_room->room_type = room_type;

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        new_room->room_connections[i] = NULL;
    }

    return new_room;
}

void room_destroy(struct Room *room)
{
    assert(room != NULL);
    free(room);
}

int count_connections(struct Room *room)
{
    int i;
    int count = 0;

    assert(room != NULL);

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

    assert(room != NULL);
    assert(connection != NULL);

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (room->room_connections[i] == connection) {
            return 0;
        }
    }
    return 1;
}

void add_connection(struct Room *room, struct Room *connection)
{
    int room_one_count = count_connections(room);
    int room_two_count = count_connections(connection);
    int connected = check_connection(room, connection);

    assert(room != NULL);
    assert(connection != NULL);

    if (room_one_count < 6 && room_two_count < 6 && connected == 1) {
        room->room_connections[room_one_count] = connection;
        connection->room_connections[room_two_count] = room;
    }
}

void room_write_to_file(struct Room *room)
{
    int i;
    FILE *fp;
    fp = fopen(room->room_name, "w+");

    assert(fp != NULL);
    assert(room != NULL);

    fprintf(fp, "ROOM NAME: %s\n", room->room_name);

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (room->room_connections[i] != NULL) {
            fprintf(fp, "CONNECTION %i: %s\n", i+1, room->room_connections[i]->room_name);
        }
    }

    fprintf(fp, "ROOM TYPE: %s\n", room->room_type);

    fclose(fp);
}

void room_load_from_file(char* file_name)
{

}


int main(int argc, char *argv[])
{
    struct Room *first_room = room_create("First room", "START_ROOM");
    struct Room *second_room = room_create("Second room", "END_ROOM");

    add_connection(first_room, second_room);

    room_write_to_file(first_room);
    room_write_to_file(second_room);

    room_destroy(first_room);
    room_destroy(second_room);

    return 0;
}
