#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
WYMAGANIA:

6 pkt. - obsługiwać dwa inne, dowolnie wybrane polecenia powłoki (chodzi tutaj np. o własną, samodzielną, średnio zaawansowaną implementację dwóch poleceń, np. cp, których kody nie były podane podczas laboratoriów, nie są rozwiązaniami zadań domowych, ani nie pochodzą ze strony Bartłomieja Przybylskiego); bardzo proste implementacje, takie jak np. echo przy pomocy printf() nie są akceptowane;

6 pkt. (*) - przyjmować polecenia odwołujące się przez nazwę do programów znajdujących się w katalogach opisanych wartością zmiennej środowiskowej PATH oraz umożliwiać wywołanie tych skryptów i programów z argumentami (czyt. fork() + exec*());

4 pkt. - wypisywać komunikat błędu, gdy niemożliwe jest poprawne zinterpretowanie polecenia;

6 pkt. (*) - posiadać tzw. dodatkowe bajery, np. wyświetlanie loginu aktualnie zalogowanego użytkownika, obsługę kolorów, obsługę argumentów w cudzysłowach, sensowną obsługę sygnałów (np. Ctrl+Z), obsługę historii poleceń poprzez strzałki, uzupełnianie składni, itp.; punkty są przyznawane w zależności od stopnia skomplikowania problemu.

ZROBIONE:

2 pkt. - wyświetlać znak zachęty w postaci [{path}] $, gdzie {path} jest ścieżką do bieżącego katalogu roboczego;

2 pkt. - obsługiwać polecenie cd, działające analogicznie jak cd znane nam z powłoki bash;

2 pkt. - obsługiwać polecenie exit, kończące działanie programu powłoki;

2 pkt. - obsługiwać polecenie help, wyświetlające na ekranie informacje o autorze programu i oferowanych przez niego funkcjonalnościach;

*/

void start();
void showCommandPrompt();
void cd(char[]);
void help();
void touch(char[]);
void cp(char[]);

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

    // Read the input
    char fullCommand[128];
    fgets(fullCommand, 128, stdin);

    // Remove the newline character from the input
    fullCommand[strcspn(fullCommand, "\n")] = '\0';

    // Extract the first word from the input
    char command[32];
    sscanf(fullCommand, "%s", command);

    // Detect the command type
    if (strcmp(command, "cd") == 0)
    {
        cd(fullCommand);
    }
    else if (strcmp(command, "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(command, "help") == 0)
    {
        help();
    }
    else if (strcmp(command, "touch") == 0)
    {
        touch(fullCommand);
    }
    else if (strcmp(command, "cp") == 0)
    {
        cp(fullCommand);
    }
    start();
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

/**
 * Same as "cd" in Bash.
 */
void cd(char fullCommand[])
{
    char path[96];
    int args = sscanf(fullCommand, "cd %s", path);
    if (args != 1)
    {
        printf("Usage: cd [path]\n");
        return;
    }

    // Check if directory exists
    if (chdir(path) != 0)
    {
        perror("cd error");
    }
}

/**
 * Prints available commands and their usages.
 */
void help()
{
    // Set visible color for the output
    printf("\033[1;33m]");

    printf("---------------------\n");
    printf("Microshell tutorial\n");
    printf("cd [path] - change current directory to [path]\n");
    printf("exit - stop the microshell\n");
    printf("touch [filename] - creates a new file with specified file name\n");
    printf("cp [source] [destination] - creates a copy of [source] file into [destination]\n");
    printf("Author: Mateusz Manczak, https://github.com/mateuszmanczak04\n");
    printf("---------------------\n");

    // Reset color
    printf("\033[0m]");
}

/**
 * Creates an empty file with specified filename.
 */
void touch(char fullCommand[])
{
    char filename[96];
    int args = sscanf(fullCommand, "touch %s", filename);
    if (args != 1)
    {
        printf("Usage: touch [filename]\n");
        return;
    }

    FILE *pFile = fopen(filename, "r");
    if (pFile != NULL)
    {
        fclose(pFile);
        printf("File '%s' already exists.\n", filename);
        return;
    }

    pFile = fopen(filename, "w");
    if (pFile == NULL)
    {
        perror("Error creating file");
        return;
    }
    fclose(pFile);
}

/**
 * Copies the contents of one file to another.
 */
void cp(char fullCommand[])
{
    char source[96], destination[96];
    int args = sscanf(fullCommand, "cp %s %s", source, destination);
    if (args != 2)
    {
        printf("Usage: cp [source] [destination]\n");
        return;
    }

    FILE *srcFile = fopen(source, "r");
    if (srcFile == NULL)
    {
        perror("Source file error");
        return;
    }

    FILE *destFile = fopen(destination, "w");
    if (destFile == NULL)
    {
        perror("Destination file error");
        fclose(srcFile);
        return;
    }

    // Maximum file size for copied files is 2048 bytes
    char buffer[2048];
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), srcFile);
    if (bytesRead > 0)
    {
        fwrite(buffer, 1, bytesRead, destFile);
    }

    fclose(srcFile);
    fclose(destFile);
}