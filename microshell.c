#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
WYMAGANIA:
6 pkt. (*) - posiadać tzw. dodatkowe bajery, np. wyświetlanie loginu aktualnie zalogowanego użytkownika, obsługę kolorów, obsługę argumentów w cudzysłowach, sensowną obsługę sygnałów (np. Ctrl+Z), obsługę historii poleceń poprzez strzałki, uzupełnianie składni, itp.; punkty są przyznawane w zależności od stopnia skomplikowania problemu.
*/

void start();
void showCommandPrompt();
void cd(char[]);
void help();
void touch(char[]);
void cp(char[]);
void otherCommand(char[]);

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
    else
    {
        otherCommand(fullCommand);
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

/**
 * Run any other command installed on the user's OS.
 */
void otherCommand(char fullCommand[])
{
    int id = fork();
    if (id == 0)
    {
        /*
            How it works:
            strtok on every iteration does something like taking the first word before
            space, adds it to the args[] array and cuts it from the original text.
            It does it as long as there are no more words in the fullCommand.
            Then it passes these arguments to the execvp() function, which simply
            runs a function from user operating system.
        */
        char *args[128];
        char *token = strtok(fullCommand, " ");
        int i = 0;
        while (token != NULL)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1)
        {
            perror("There was an error with execvp() function.");
        }
        exit(EXIT_FAILURE);
    }
    else if (id > 0)
    {
        // Wait untile the child process is done
        wait(NULL);
    }
    else
    {
        perror("There was an error when creating a child process with fork().");
    }
}