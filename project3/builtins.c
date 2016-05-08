#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void change_directory(const char* path)
{
    char* dir;

    dir = getenv("HOME");
    if (path != NULL) {
        dir = path;
    }

    chdir(dir);
}
