#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // getpid()
#include <sys/stat.h> // mkdir()
#include <assert.h>
#include <string.h>


#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6
#define ROOM_CHOICES 10
#define GAME_ROOMS 7

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

    int different = room == connection ? 1 : 0;
    int room_one_count = count_connections(room);
    int room_two_count = count_connections(connection);
    int connected = check_connection(room, connection);

    if (room_one_count < 6 &&
        room_two_count < 6 &&
        connected     == 1 &&
        different     == 0) {
        room->room_connections[room_one_count] = connection;
        connection->room_connections[room_two_count] = room;
    }
}

void room_write_to_file(struct Room *room)
{
    int i;
    FILE *fp;

    fp = fopen(room->room_name, "w+"); // PERMISSIONS // check not null

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
    int lenstr = strlen(str);
    int lenpre = strlen(pre);

    return lenstr < lenpre ? 1 : strncmp(pre, str, lenpre) != 0;
}

// http://stackoverflow.com/questions/6127503/shuffle-array-in-c
void shuffle(int *array, size_t n)
{
    int i, j;
    int t;

    if (n > 1) {
        for (i = 0; i < n  - 1; i++) {
            j = i + rand() / (RAND_MAX / (n - i) + 1);
            t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

// http://stackoverflow.com/questions/1608181/unique-random-numbers-in-an-integer-array-in-the-c-programming-language
// http://www.tutorialspoint.com/cprogramming/c_return_arrays_from_function.htm
// http://stackoverflow.com/questions/8314370/rand-with-seed-does-not-return-random-if-function-looped
int* choose_random_indices()
{
    int in, im;
    static int indices[GAME_ROOMS];

    im = 0;

    for (in = 0; in < ROOM_CHOICES && im < GAME_ROOMS; in++) {
        int rn = ROOM_CHOICES - in;
        int rm = GAME_ROOMS - im;
        if (rand() % rn < rm)
            indices[im++] = in + 1;
    }

    assert(im = GAME_ROOMS);
    shuffle(indices, GAME_ROOMS);
    return indices;
}

void make_temp_dir(char* dir_name)
{
    int stat;
    // http://pubs.opengroup.org/onlinepubs/009695399/functions/mkdir.html
    stat = mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    assert(stat == 0);
}

void connect_rooms(struct Room** rooms)
{
    // randomly create connections
    // choose a random number n between 3 and 6
    // choose n indices to connect to
    // do until at least n connections are made
    int i, j, num_connections;

    j = 0;

    for (i = 0; i < GAME_ROOMS; i++) {
        num_connections = rand() % (MAX_CONNECTIONS - MIN_CONNECTIONS) + MIN_CONNECTIONS;
        while (count_connections(rooms[i]) < num_connections) {
            j = rand() % GAME_ROOMS;
            add_connection(rooms[i], rooms[j]);
        }
    }
}

void create_room_files(struct Room **rooms)
{
   int i;
   for (i = 0; i < GAME_ROOMS; i++) {
        room_write_to_file(rooms[i]);
    }
}

const char* set_up()
{
    int i;
    int* random_indices;
    char* room_type;
    const char* start_room;
    struct Room *rooms[GAME_ROOMS];

    // randomly pick 7 room names
    random_indices = choose_random_indices(ROOM_CHOICES, GAME_ROOMS);

    for (i = 0; i < GAME_ROOMS; i++) {
        switch (i) {
            case 0:
                room_type = "START_ROOM";
                start_room = ROOM_NAMES[random_indices[i] - 1];
                break;
            case GAME_ROOMS - 1:
                room_type = "END_ROOM";
                break;
            default:
                room_type="MID_ROOM";
        }
        rooms[i] = room_create(ROOM_NAMES[random_indices[i] - 1], room_type);
    }

    connect_rooms(rooms);
    create_room_files(rooms);

    // destroy room structs
    for (i = 0; i < (sizeof(rooms) / sizeof(struct Room*)); i++) {
        room_destroy(rooms[i]);
    }

    return start_room;
}

const char* next_room(const char* file_name)
{
    char str[30];
    FILE *fp;
    const char* ptr;
    const char* connections[6];
    int i = 0;
    fp = fopen(file_name, "r");  // check fp
    assert(fp != NULL);

    // http://stackoverflow.com/questions/1479386/is-there-a-function-in-c-that-will-return-the-index-of-a-char-in-a-char-array
    while (fgets(str, sizeof str, fp) != NULL) {
        ptr = strchr(str, ':');
        if (starts_with(str, "ROOM NAME") == 0 && ptr) {
            printf("%s\n", ptr+2);
        }
        if (starts_with(str, "CONNECTION") == 0 && ptr) {
            // printf("%s\n", ptr+2);
            connections[i] = ptr+2; // NOPE DOESN'T WORK
            i++;
        }
        if (starts_with(str, "ROOM TYPE") == 0 && ptr) {
            printf("%s\n", ptr+2);
        }
    }
    for (i = 0; i < 6; i++) {
        if (connections[i] != NULL) {
            printf("%s\n", connections[i]);
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int pid;
    int seed;
    int stat;
    char* dir_name;
    const char* start;

    pid = getpid();
    dir_name = malloc(sizeof(char) * 30);
    sprintf(dir_name, "mcginnit.rooms.%i", pid);
    make_temp_dir(dir_name);

    stat = chdir(dir_name);
    assert(stat == 0);

    free(dir_name);

    seed = time(NULL);
    srand(seed);
    start = set_up();
    next_room(start);

    return 0;
}
