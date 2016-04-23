#include "dbg.h"    // REMOVE ME
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define MAX_CONNECTIONS 6
#define MAX_NAME_LENGTH 10  // includes extra byte for \0

const char* ROOM_NAMES[] = {
    "Coruscant",
    "Tatooine",
    "Felucia",
    "Hoth",
    "Naboo",
    "Serenno",
    "Kamino",
    "Kashyyyk",
    "Mandalore",
    "Ryloth"
};

struct Room {
    const char* room_name;
    struct Room *room_connections[MAX_CONNECTIONS];
    const char* room_type;
};

struct Room *room_create(const char* room_name, const char* room_type)
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

// http://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c
int starts_with(const char* str, const char* pre)
{
    size_t lenstr = strlen(str);
    size_t lenpre = strlen(pre);

    return lenstr < lenpre ? 1 : strncmp(pre, str, lenpre) != 0;
}

struct Room *room_read_from_file(const char* file_name)
{
    int i;
    int lenpre;
    char str[30];
    char* connections[6];
    FILE *fp;
    fp = fopen(file_name, "r");
    assert(fp != NULL);

    // get file name
    fseek(fp, 11, SEEK_CUR);
    fgets(str, sizeof(str), fp);
    printf("CURRENT LOCATION: %s", str);

    fclose(fp);
}


int main(int argc, char *argv[])
{
    struct Room *first_room = room_create(ROOM_NAMES[0], "START_ROOM");
    struct Room *second_room = room_create(ROOM_NAMES[1], "END_ROOM");
    struct Room *third_room = room_create(ROOM_NAMES[2], "MID_ROOM");

    add_connection(first_room, second_room);
    add_connection(first_room, third_room);

    room_write_to_file(first_room);
    room_write_to_file(second_room);

    room_read_from_file(first_room->room_name);

    room_destroy(first_room);
    room_destroy(second_room);
    room_destroy(third_room);

    return 0;
}
