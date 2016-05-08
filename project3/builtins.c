#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


static int STATUS = 0;

void change_directory(char* path)
{
    char* dir;

    dir = getenv("HOME");
    if (path != NULL) {
        dir = path;
    }

    chdir(dir);
}

int get_status()
{
    return STATUS;
}

void set_status(int new_status)
{
    STATUS = new_status;
}
