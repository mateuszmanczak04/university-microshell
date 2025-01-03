#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define BUF_SIZE 1024

int main()
{
    char cwd[BUF_SIZE];
    char command[BUF_SIZE];
    char *args[MAX_ARGS];

    while (1)
    {
        // Display prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("[%s] $ ", cwd);
        }
        else
        {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

        // Read input
        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            printf("\n");
            break;
        }
        // Remove trailing newline
        command[strcspn(command, "\n")] = '\0';

        // Skip empty input
        if (strlen(command) == 0)
        {
            continue;
        }

        // Parse input into arguments
        int argc = 0;
        char *token = strtok(command, " ");
        while (token != NULL && argc < MAX_ARGS - 1)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        // Handle built-in commands
        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }
        else if (strcmp(args[0], "cd") == 0)
        {
            if (argc < 2)
            {
                fprintf(stderr, "microshell: expected argument to \"cd\"\n");
            }
            else
            {
                if (chdir(args[1]) != 0)
                {
                    perror("microshell");
                }
            }
        }
        else if (strcmp(args[0], "help") == 0)
        {
            printf("Microshell by [Your Name]\n");
            printf("Supported commands:\n");
            printf("  cd [directory] - change directory\n");
            printf("  exit - exit the shell\n");
            printf("  help - display this help message\n");
            printf("  [command] - execute commands from PATH\n");
        }
        else
        {
            // Execute external command
            pid_t pid = fork();
            if (pid == 0)
            {
                // Child process
                execvp(args[0], args);
                // If execvp returns, an error occurred
                perror("microshell");
                exit(EXIT_FAILURE);
            }
            else if (pid < 0)
            {
                // Fork failed
                perror("microshell");
            }
            else
            {
                // Parent process
                waitpid(pid, NULL, 0);
            }
        }
    }
}