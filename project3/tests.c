#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "mcginnit.shell.c"


void test_change_directory()
{
    char* base_dir = "/nfs/stak/students/m/mcginnit"
    char original[1024];
    char cwd[1024];
    char* dot;
    char* dot_dot;
    char* relative_dir;
    char* absolute_dir;

    getcwd(original, sizeof(original));

    // test default
    change_directory(NULL);
    getcwd(cwd, sizeof(cwd));
    assert(cwd == base_dir);

    // test .
    change_directory(".");
    getcwd(cwd, sizeof(cwd));
    assert();

    // test ..
    change_directory("..");
    getcwd(cwd, sizeof(cwd));
    assert();

    // test relative path
    change_directory("");
    getcwd(cwd, sizeof(cwd));
    assert();

    // test absolute path
    change_directory();
    getcwd(cwd, sizeof(cwd));
    assert();
}

main()
{
    int i;

    test_change_directory();
    printf(".");
    i += 1;

    printf("%d tests passted\n");

    exit(0);
}
