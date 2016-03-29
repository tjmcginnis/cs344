#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct Person {
    char *name;
    int age;
};

struct Person *createPerson(char *name, int age) {
    struct Person *p = malloc(sizeof(struct Person));
    assert (p != NULL);
    p->name = strdup(name);
    p->age = age;
    return p;
}

struct Person alternateCreatePerson(char *name, int age) {
    struct Person p;
    p.name = name;
    p.age = age;
    return p;
}

void deletePerson(struct Person *p) {
    assert (p != NULL);
    free(p->name);
    free(p);
}

void printPerson(struct Person *p) {
    printf("%s: %i\n", p->name, p->age);
}

int main(int argc, char *argv[]) {
    int i;
    int myVar = 5;
    int *myPtr = &myVar;
    int myArr[] = {1, 2, 3, 4, 5};
    char *myName = "Tyler";
    char otherName[] = "Lee";

    struct Person *me = createPerson(myName, 27);
    printPerson(me);

    struct Person lee = alternateCreatePerson(otherName, 28);
    printPerson(&lee);

    myVar += 1;

    printf("%i, %s, %s\n", myVar, myName, otherName);
    printf("%i\n", *myPtr);
    printf("%p\n", myPtr);
    printf("%p\n", &myVar);

    for (i = 0; i < sizeof(myArr) / sizeof(int); i++) {
        printf("%i\n", myArr[i]);
    }
    return 0;
}
