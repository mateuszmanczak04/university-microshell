#include <stdio.h>

void printAge(int *pAge)
{
    printf("You are %d years old\n", *pAge);
    *pAge += 1;
}

int main()
{
    int age = 21;
    int *pAge = NULL; // Good practice to do it in 2 lines
    pAge = &age;

    // printf("address of age: %p\n", &age);
    // printf("address of age: %p\n", pAge);

    // printf("size of age: %d bytes\n", sizeof(age));
    // printf("size of pAge: %d bytes\n", sizeof(pAge));

    // printf("value of age %d\n", age);
    // printf("value at stored address %d\n", *pAge);

    printAge(&age);
    printAge(pAge);

    return 0;
}