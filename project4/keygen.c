#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char* VALID_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int main(int argc, char* argv[])
{
    FILE* fd;
    char* key;
    int i, keylength;

    if (argc < 2) {
        perror("keylength not supplied");
        exit(1);
    }

    // get keylength from argv
    sscanf(argv[1], "%d", &keylength);
    if (keylength == 0) {
        perror("keylength supplied is not an integer");
        exit(1);
    }

    key = malloc(sizeof(char) * keylength + 2);

    for (i = 0; i < keylength; i++) {
        /*
         * Assign a random char from VALID_CHARS to each index of key
         * http://stackoverflow.com/questions/19724346/generate-random-characters-in-c
         */
        key[i] = VALID_CHARS[random() % strlen(VALID_CHARS)];
    }

    key[keylength] = '\n';  // add newline
    key[keylength + 1] = 0; // add null terminator

    write(STDOUT_FILENO, key, strlen(key));  // write key to stdout

    free(key);

    exit(0);
}
