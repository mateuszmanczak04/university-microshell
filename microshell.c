#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
WYMAGANIA:

2 pkt. - obsługiwać polecenie cd, działające analogicznie jak cd znane nam z powłoki bash;

2 pkt. - obsługiwać polecenie exit, kończące działanie programu powłoki;

2 pkt. - obsługiwać polecenie help, wyświetlające na ekranie informacje o autorze programu i oferowanych przez niego funkcjonalnościach;

6 pkt. - obsługiwać dwa inne, dowolnie wybrane polecenia powłoki (chodzi tutaj np. o własną, samodzielną, średnio zaawansowaną implementację dwóch poleceń, np. cp, których kody nie były podane podczas laboratoriów, nie są rozwiązaniami zadań domowych, ani nie pochodzą ze strony Bartłomieja Przybylskiego); bardzo proste implementacje, takie jak np. echo przy pomocy printf() nie są akceptowane;

6 pkt. (*) - przyjmować polecenia odwołujące się przez nazwę do programów znajdujących się w katalogach opisanych wartością zmiennej środowiskowej PATH oraz umożliwiać wywołanie tych skryptów i programów z argumentami (czyt. fork() + exec*());

4 pkt. - wypisywać komunikat błędu, gdy niemożliwe jest poprawne zinterpretowanie polecenia;

6 pkt. (*) - posiadać tzw. dodatkowe bajery, np. wyświetlanie loginu aktualnie zalogowanego użytkownika, obsługę kolorów, obsługę argumentów w cudzysłowach, sensowną obsługę sygnałów (np. Ctrl+Z), obsługę historii poleceń poprzez strzałki, uzupełnianie składni, itp.; punkty są przyznawane w zależności od stopnia skomplikowania problemu.

ZROBIONE:

2 pkt. - wyświetlać znak zachęty w postaci [{path}] $, gdzie {path} jest ścieżką do bieżącego katalogu roboczego;

*/

void start();
void showCommandPrompt();

int main()
{
    // Infinite app loop
    while (1)
    {
        start();
    }
    return 0;
}

/**
 * Parse input and run following commands.
 */
void start()
{
    showCommandPrompt();

    char fullCommand[128];

    // Read the input
    fgets(fullCommand, 128, stdin);
    printf("Full command: %s\n", fullCommand);

    // Remove the newline character from the input
    fullCommand[strcspn(fullCommand, "\n")] = '\0';

    // Extract the first word from the input
    char command[32];
    sscanf(fullCommand, "%s", command);

    printf("Command: %s\n", command);
}

/**
 * Just print the prompt sign.
 */
void showCommandPrompt()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("[%s] $ ", cwd);
    }
    else
    {
        perror("getcwd() error");
    }
}