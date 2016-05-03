/*
 * Author: Tyler McGinnis
 * CS 344, Section 400
 * Spring 2016
 *
 * file: mcginnit.adventure.c
 * description:
 *  a Star Wars themed adventure game
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>


#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6
#define ROOM_CHOICES 10
#define GAME_ROOMS 7
#define NAME_LENGTH 11


/* Potential room names for game rooms */
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

/* A game room */
struct Room {
    const char  *room_name;
    struct Room *room_connections[MAX_CONNECTIONS];
    const char  *room_type;
};


/* A game instance */
struct Game {
    int num_steps;
    int finished;
    char *current_room;
    char *next_room_choices[MAX_CONNECTIONS];
    char *path_file_name;
    FILE *path_file;
};


/*
 * Create a new struct Room *
 *
 * @param room_name
 *  The room's name
 * @param room_type
 *  The room's type (START_ROOM, MID_ROOM, or END_ROOM)
 *
 * Return a pointer to a struct Room
 */
struct Room *room_create(const char *room_name, const char *room_type)
{
    int i;
    struct Room *new_room = malloc(sizeof(struct Room));

    assert(new_room != NULL);  // check malloc successful

    new_room->room_name = room_name;
    new_room->room_type = room_type;

    /*
     * Initialize room's connections to NULL
     * Later used to check that a connection is present
     */
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        new_room->room_connections[i] = NULL;
    }

    return new_room;
}


/*
 * Destroy a struct Room *
 *
 * @param room
 *  The room to destroy
 */
void room_destroy(struct Room *room)
{
    assert(room != NULL);
    free(room);
}


/*
 * Count the number of connections to a room
 *
 * @param room
 *  The room to count connections to
 *
 * Return an integer representing the number of connections
 */
int count_connections(struct Room *room)
{
    int i, count;

    count = 0;
    assert(room != NULL);

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (room->room_connections[i] != NULL) {  // check connection exists
            count += 1;
        }
    }
    return count;
}


/*
 * Check if two rooms are connected
 *
 * @param room
 *  The first room
 * @param connection
 *  The second room
 *
 * Return 0 if connected, 1 otherwise
 */
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


/*
 * Connect two rooms
 *
 * @param room
 *  The first room
 * @param connection
 *  The second room
 */
void add_connection(struct Room *room, struct Room *connection)
{
    int different, room_one_count, room_two_count, connected;

    assert(room != NULL);
    assert(connection != NULL);

    /* check that room and connection are not the same */
    different = room == connection ? 1 : 0;
    /* count room's connection */
    room_one_count = count_connections(room);
    /* count connection's connection */
    room_two_count = count_connections(connection);
    /* check if room and connection are already connected */
    connected = check_connection(room, connection);

    if (room_one_count < MAX_CONNECTIONS &&
        room_two_count < MAX_CONNECTIONS &&
        connected     == 1 &&
        different     == 0) {
        /*
         * if rooms are:
         *  unique
         *  have less than MAX_CONNECTIONS
         *  are not already connected
         * then create mutual connection
         * */
        room->room_connections[room_one_count] = connection;
        connection->room_connections[room_two_count] = room;
    }
}


/*
 * Write a room's data to a file
 *
 * @param room
 *  The room to write
 */
void room_write_to_file(struct Room *room)
{
    int i;
    FILE *fp;

    fp = fopen(room->room_name, "w+"); // PERMISSIONS // check not null

    assert(fp   != NULL);
    assert(room != NULL);

    /* write room name to file */
    fprintf(fp, "ROOM NAME: %s\n", room->room_name);

    /* write each connecting room to file on separate lines */
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (room->room_connections[i] != NULL) {
            fprintf(fp,
                    "CONNECTION %i: %s\n",
                    i+1,
                    room->room_connections[i]->room_name
            );
        }
    }

    /* write room type to file */
    fprintf(fp, "ROOM TYPE: %s\n", room->room_type);

    fclose(fp);
}


/*
 * Create a new game instance
 *
 * @param start_room
 *  The starting room for the game
 */
struct Game *game_create(const char *start_room)
{
    int i;

    struct Game* new_game = malloc(sizeof(struct Game));
    assert(new_game != NULL);

    new_game->num_steps = 0;
    new_game->finished = 0;

    new_game->current_room = malloc(sizeof(char) * NAME_LENGTH);
    assert(new_game->current_room != NULL);
    /* current room is the starting room */
    memcpy(new_game->current_room, start_room, NAME_LENGTH);

    /* create file to store path traversed */
    new_game->path_file_name = "victory_path";
    new_game->path_file = fopen(new_game->path_file_name, "a+");

    /* allocate space for MAX_CONNECTIONS room choices */
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        new_game->next_room_choices[i] = malloc(sizeof(char) * NAME_LENGTH);
        assert(new_game->next_room_choices[i] != NULL);
        /* initialize allocated memory to \0 */
        memset(new_game->next_room_choices[i], 0, NAME_LENGTH);
    }

    return new_game;
}


/*
 * Reset game instance's room choices to \0
 *
 * @param game
 *  game instance
 */
void game_clear_room_choices(struct Game *game)
{
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        memset(game->next_room_choices[i], 0, NAME_LENGTH);
    }
}


/*
 * Destroy a game instance
 *
 * @param game
 *  game instance to destroy
 */
void game_destroy(struct Game *game)
{
    int i, stat;

    assert(game != NULL);
    assert(game->current_room != NULL);
    assert(game->path_file != NULL);

    /* free room choices */
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        assert(game->next_room_choices[i] != NULL);
        free(game->next_room_choices[i]);
    }

    /* close and delete game's path file */
    fclose(game->path_file);
    stat = unlink(game->path_file_name);
    assert(stat == 0);

    free(game->current_room);
    free(game);
}


/*
 * Add a room to a game instance's room choices
 *
 * @param game
 *  game instance
 * @param room
 *  room to add as a choice
 * @param index
 *  index of next_room_choices to add room to
 */
void game_add_to_next_room_choices(struct Game *game, const char *room, int index)
{
    /* first clear the memory that will be used */
    memset(game->next_room_choices[index], 0, NAME_LENGTH);
    /* copy room name into next_room_choices */
    memcpy(game->next_room_choices[index], room, NAME_LENGTH);
    /*
     * remove newline characters
     * (in cases where room comes from standard input)
     * */
    game->next_room_choices[index] =
        strtok(game->next_room_choices[index], "\n");
}


/*
 * Set a game instance's current room
 *
 * @param game
 *  game instance
 * @param next_room
 *  the next current room for the game
 */
void game_set_current_room(struct Game* game, char *next_room)
{
    /* first clear the memory to be used */
    memset(game->current_room, 0, NAME_LENGTH);
    memcpy(game->current_room, next_room, NAME_LENGTH);
}


/*
 * Add a room to the game's path
 *
 * @param game
 *  game instance
 * @param room_name
 *  room to add
 */
void game_add_to_path(struct Game *game, const char *room_name)
{
    int stat;

    stat = fputs(room_name, game->path_file);  // append to file
    assert(stat >= 0);
    stat = fputs("\n", game->path_file);  // add a newline
    assert(stat >= 0);
}


/*
 * Print the path traversed through a game
 *
 * @param game
 *  game instance
 */
void game_print_path(struct Game *game)
{
    char str[30];

    rewind(game->path_file);  // rewind to beginning of file
    /* print file line-by-line */
    while (fgets(str, sizeof str, game->path_file) != NULL) {
        printf("%s", str);
    }
}


/*
 * Check if user choice is a valid room choice
 *
 * @param next_room
 *  the user's choice
 * @param game
 *  the game instance
 *
 * Return 0 if valid choice, 1 otherwise
 */
int check_choice(char *next_room, struct Game *game)
{
    int i;
    char *choice;

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        choice = game->next_room_choices[i];
        /*
         * Compare choice with connecting room name
         * Ensure connecting room is not an empty string
         * */
        if (starts_with(next_room, choice) == 0 && strcmp(choice, "") != 0) {
            return 0;
        }
    }
    return 1;
}


/*
 * Check if one string begins with another string
 *
 * @param str
 *  The string to check
 * @param
 *  The prefix to check if str begins with
 *
 * Return 0 if a match, 1 otherwise
 *
 * starts_with based on:
 * http://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c
 */
int starts_with(const char *str, const char *pre)
{
    int lenstr = strlen(str);
    int lenpre = strlen(pre);

    return lenstr < lenpre ? 1 : strncmp(pre, str, lenpre) != 0;
}


/*
 * Shuffle an array of integers
 *
 * @param array
 *  An array of integers
 * @param n
 *  The size of the array
 *
 * shuffle based on:
 * http://stackoverflow.com/questions/6127503/shuffle-array-in-c
 */
void shuffle(int *array, int n)
{
    int i, j, t;

    if (n > 1) {
        for (i = 0; i < n  - 1; i++) {
            j = i + rand() / (RAND_MAX / (n - i) + 1);
            t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}


/*
 * Create an array of random indices
 * Chooses GAME_ROOM numbers between 1 and ROOM_CHOICES
 *
 * Return array of unique random numbers
 *
 * choose_random_indices based on:
 * http://stackoverflow.com/questions/1608181/unique-random-numbers-in-an-integer-array-in-the-c-programming-language
 * http://www.tutorialspoint.com/cprogramming/c_return_arrays_from_function.htm
 * http://stackoverflow.com/questions/8314370/rand-with-seed-does-not-return-random-if-function-looped
 */
int *choose_random_indices()
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


/*
 * Create a new directory
 *
 * @param dir_name
 *  Directory name to use
 *
 * mkdir usage based on:
 * http://pubs.opengroup.org/onlinepubs/009695399/functions/mkdir.html
 */
void make_temp_dir(char *dir_name)
{
    int stat;

    stat = mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    assert(stat == 0);
}


/*
 * Randomly create a random number of connections between
 * an array of struct Rooms
 *
 * @param rooms
 *  Array of struct rooms to create connections between
 */
void connect_rooms(struct Room **rooms)
{
    int i, j, num_connections;

    j = 0;

    for (i = 0; i < GAME_ROOMS; i++) {
        /* randomly choose a number of connections for room */
        num_connections = rand() %
            (MAX_CONNECTIONS - MIN_CONNECTIONS) + MIN_CONNECTIONS;
        /* Add connections until chosen number of connections made */
        while (count_connections(rooms[i]) < num_connections) {
            /*
             * Choose random index and connect room at that index to current
             * room
             */
            j = rand() % GAME_ROOMS;
            add_connection(rooms[i], rooms[j]);
        }
    }
}


/*
 * Write each room from an array of rooms to a file
 *
 * @param rooms
 *  array of rooms
 */
void create_room_files(struct Room **rooms)
{
   int i;
   for (i = 0; i < GAME_ROOMS; i++) {
        room_write_to_file(rooms[i]);
    }
}


/*
 * Set up the files needed for gameplay
 *
 * Returns the name of the room designated as
 * the starting room
 */
const char *set_up()
{
    int i;
    int *random_indices;
    char *room_type;
    const char *start_room;
    struct Room *rooms[GAME_ROOMS];

    /* randomly pick integers, which will be used to pick room names */
    random_indices = choose_random_indices(ROOM_CHOICES, GAME_ROOMS);

    /* create rooms */
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

    /* make random connections between rooms */
    connect_rooms(rooms);

    /* write room data to files */
    create_room_files(rooms);

    /* destroy struct Rooms */
    for (i = 0; i < (sizeof(rooms) / sizeof(struct Room*)); i++) {
        room_destroy(rooms[i]);
    }

    return start_room;
}


/*
 * Read in the current rooms data and prompt user
 * Set the current room to the next room
 *
 * @param game
 *  game instance to use
 *
 *  use of strchar based on:
 *  http://stackoverflow.com/questions/1479386/is-there-a-function-in-c-that-will-return-the-index-of-a-char-in-a-char-array
 */
void next_room(struct Game *game)
{
    int i, j, keep_choosing;
    FILE *fp;
    char line[30], buffer[256];
    char *next;
    const char *ptr;

    i = 0;
    keep_choosing = 1;
    /* open room file */
    fp = fopen(game->current_room, "r");
    assert(fp != NULL);

    /* read room file line-by-line */
    while (fgets(line, sizeof line, fp) != NULL) {
        /* get a pointer to the line's data */
        ptr = strchr(line, ':');
        ptr += 2;
        if (starts_with(line, "CONNECTION") == 0 && ptr) {
            game_add_to_next_room_choices(game, ptr, i);
            i += 1;  // i keeps track of the next choice index
        }
        if (starts_with(line, "ROOM TYPE") == 0 && ptr) {
            if (starts_with(ptr, "END_ROOM") == 0) {
                /* game over */
                game->finished = 1;
                keep_choosing = 0;
            }
        }
    }

    /* keep user choosing until a valid choice is entered */
    while(keep_choosing == 1) {
        fseek(stdin, 0, SEEK_END);
        printf("CURRENT LOCATION: %s\n", game->current_room);
        printf("POSSIBLE CONNECTIONS: ");
        for (j = 0; j < i; j++) {
            printf("%s", game->next_room_choices[j]);
            /* Proper grammar and formatting */
            if (j < i - 1) {
                printf(", ");
            } else {
                printf(".\n");
            }
        }

        printf("WHERE TO? >");
        fgets(buffer, sizeof(buffer), stdin);  // get user's choice

        if (check_choice(buffer, game) == 0) {
            /* valid choice is the new current room */
            game_set_current_room(game, strtok(buffer, "\n"));
            game_add_to_path(game, game->current_room);
            keep_choosing = 0;
            game->num_steps += 1;
        } else {
            printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
        }
        printf("\n");
    }

    game_clear_room_choices(game);  // reset room choices
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int pid;
    int seed;
    int stat;
    char *dir_name;
    const char *start;
    struct Game *game;

    /* Create instance directory */
    pid = getpid();
    dir_name = malloc(sizeof(char) * 30);
    sprintf(dir_name, "mcginnit.rooms.%i", pid);
    make_temp_dir(dir_name);

    /* Change to instance directory */
    stat = chdir(dir_name);
    assert(stat == 0);

    free(dir_name);

    /* Seed rand */
    seed = time(NULL);
    srand(seed);

    /* Get first room and create game */
    start = set_up();
    game = game_create(start);

    do {
        next_room(game);
    } while (game->finished == 0);

    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", game->num_steps);
    game_print_path(game);

    game_destroy(game);

    exit(0);
}
