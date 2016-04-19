#include "dbg.h"    // REMOVE ME
#include <stdio.h>
#include <stdlib.h>


struct Room {
    char *room_name;
    struct Room *connections;
};

int main(int argc, char *argv[])
{
    char *name = "Champagne Room";
    char *other_name = "Bubbly room";

    struct Room *new_room = malloc(sizeof(struct Room));
    new_room->room_name = name;
    new_room->connections = malloc(6 * sizeof(struct Room));

    struct Room *other_room = malloc(sizeof(struct Room));
    other_room->room_name = other_name;
    new_room->connections[0] = *other_room;
    struct Room *other_other = new_room->connections[0];

    printf("Room name: %s\n", new_room->room_name);
    printf("Other room name: %s\n", other_other->room_name);

    free(new_room);

    return 0;
}
