#include <stdio.h>
#include <string.h>

// input using spaces -> fget
// input not using spaces -> scanf

int main()
{
    char name[25]; // bytes
    int age;

    printf("What's your name? ");
    fgets(name, 25, stdin);
    name[strlen(name)-1] = '\0';

    printf("How old are you? ");
    scanf("%d", &age);

    printf("Hello %s, how are you?", name);
    printf("\nYou are %d years old.", age);

    printf("\n");

    return 0;
}