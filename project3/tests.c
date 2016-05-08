#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "builtins.h"


void test_change_directory()
{
    char* home = "/nfs/stak/students/m/mcginnit";
    char* dot = "/nfs/stak/students/m/mcginnit/CS344/project3";
    char* dot_dot = "/nfs/stak/students/m/mcginnit/CS344";
    char* relative = "/nfs/stak/students/m/mcginnit/CS344/project2";
    char* absolute = "/nfs/stak/students/m/mcginnit/CS344/project2";
    char original[1024];
    char cwd[1024];

    getcwd(original, sizeof(original));

    // test default
    change_directory(NULL);
    getcwd(cwd, sizeof(cwd));
    assert(strncmp(cwd, home, strlen(home)) == 0);

    memset(cwd, 0, strlen(cwd));
    chdir(original);

    // test .
    change_directory(".");
    getcwd(cwd, sizeof(cwd));
    assert(strncmp(cwd, dot, strlen(dot)) == 0);

    memset(cwd, 0, strlen(cwd));
    chdir(original);

    // test ..
    change_directory("..");
    getcwd(cwd, sizeof(cwd));
    assert(strncmp(cwd, dot_dot, strlen(dot)) == 0);

    memset(cwd, 0, strlen(cwd));
    chdir(original);

    // test relative path
    change_directory("../project2");
    getcwd(cwd, sizeof(cwd));
    assert(strncmp(cwd, relative, strlen(dot)) == 0);

    memset(cwd, 0, strlen(cwd));
    chdir(original);

    // test absolute path
    change_directory(absolute);
    getcwd(cwd, sizeof(cwd));
    assert(strncmp(cwd, absolute, strlen(dot)) == 0);
}

int main(int argc, char *argv[])
{
    int i = 0;

    test_change_directory();
    printf(" . ");
    i += 1;

    printf("\n---------------------------\n");
    printf("%d tests passted\n", i);

    exit(0);
}
