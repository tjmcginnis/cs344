#include "dbg.h"    // REMOVE ME
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // getpid()
#include <sys/stat.h> // mkdir()
#include <assert.h>


#define MAX_CONNECTIONS 6
#define ROOM_CHOICES 10
#define GAME_ROOMS 7
#define START_ROOMS 1
#define END_ROOMS 1

const char* ROOM_NAMES[ROOM_CHOICES] = {
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

const char* ROOM_TYPES[] = {
    "START_ROOM",
    "MID_ROOM",
    "END_ROOM"
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
    assert(room != NULL);
    assert(connection != NULL);

    int room_one_count = count_connections(room);
    int room_two_count = count_connections(connection);
    int connected = check_connection(room, connection);

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

// http://stackoverflow.com/questions/1608181/unique-random-numbers-in-an-integer-array-in-the-c-programming-language
// http://www.tutorialspoint.com/cprogramming/c_return_arrays_from_function.htm
// http://stackoverflow.com/questions/8314370/rand-with-seed-does-not-return-random-if-function-looped
int* choose_random_indices()
{
    int in, im;
    int seed;
    static int indices[GAME_ROOMS];

    seed = time(NULL);
    srand(seed);
    im = 0;

    for (in = 0; in < ROOM_CHOICES && im < GAME_ROOMS; in++) {
        int rn = ROOM_CHOICES - in;
        int rm = GAME_ROOMS - im;
        if (rand() % rn < rm)
            indices[im++] = in + 1;
    }

    assert(im = GAME_ROOMS);
    return indices;
}

const char* set_up()
{
    int i;
    int pid;
    int stat;
    int* indices;
    char* dir_name;
    const char* start_room;
    const char* game_rooms[GAME_ROOMS];

    // create tmp dir with process id
    pid = getpid();
    dir_name = malloc(sizeof(char) * 30);
    sprintf(dir_name, "mcginnit.rooms.%i", pid);
    // http://pubs.opengroup.org/onlinepubs/009695399/functions/mkdir.html
    stat = mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    assert(stat == 0);

    // randomly pick 7 room names
    indices = choose_random_indices();

    for (i = 0; i < GAME_ROOMS; i++) {
        game_rooms[i] = ROOM_NAMES[indices[i] - 1];
    }

    // create rooms

    // randomly create connections

    // write each room to a file

    // destroy room structs

    free(dir_name);

    return start_room;
}

const char* next_room(const char* file_name)
{
    char str[30];
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
    struct Room *fourth_room = room_create(ROOM_NAMES[3], "MID_ROOM");
    struct Room *fifth_room = room_create(ROOM_NAMES[4], "MID_ROOM");
    struct Room *sixth_room = room_create(ROOM_NAMES[5], "MID_ROOM");
    struct Room *seventh_room = room_create(ROOM_NAMES[6], "MID_ROOM");

    add_connection(first_room, second_room);
    add_connection(first_room, third_room);

    room_write_to_file(first_room);
    room_write_to_file(second_room);

    room_destroy(first_room);
    room_destroy(second_room);
    room_destroy(third_room);
    room_destroy(fourth_room);
    room_destroy(fifth_room);
    room_destroy(sixth_room);
    room_destroy(seventh_room);

    set_up();

    return 0;
}
