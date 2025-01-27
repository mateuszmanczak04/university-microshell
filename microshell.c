#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <termios.h>
#include <ctype.h>

// How long we can go to the past (32x arrow up)
#define HISTORY_SIZE 32

char history[HISTORY_SIZE][128];
int history_count = 0;
int history_index = -1;

// Handling history
void enableRawMode();
void disableRawMode();
void addToHistory(char *);
void getCommandFromHistory(char *, int);
void readInput(char *);

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
    readInput(fullCommand);

    // Replace multiple whitespaces with a single space and trim the command
    char *src = fullCommand, *dst = fullCommand;
    int in_whitespace = 0;

    // Trim leading whitespace
    while (*src && isspace((unsigned char)*src)) {
        src++;
    }

    // Process the command
    while (*src) {
        if (isspace((unsigned char)*src)) {
            if (!in_whitespace) {
                *dst++ = ' ';
                in_whitespace = 1;
            }
        } else {
            *dst++ = *src;
            in_whitespace = 0;
        }
        src++;
    }

    // Trim trailing whitespace
    if (dst > fullCommand && isspace((unsigned char)*(dst - 1))) {
        dst--;
    }
    *dst = '\0';

    // Add command to history
    addToHistory(fullCommand);

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
    // TODO: when file exists it should change last modified date
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
void otherCommand(char fullCommand[]) {
    // Ignore SIGINT and SIGTSTP in the parent process
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    int id = fork();
    if (id == 0) {
        // In the child process, restore default behavior for SIGINT and SIGTSTP
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        char *args[128];
        char *token = strtok(fullCommand, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1) {
            perror("There was an error with execvp() function.");
        }
        exit(EXIT_FAILURE);
    } else if (id > 0) {
        // Parent process: wait for the child to finish
        int status;
        waitpid(id, &status, WUNTRACED); // Wait for termination or suspension (SIGTSTP)

        // Restore SIGINT and SIGTSTP handling for the parent process
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        // If the child was stopped (e.g., Ctrl+Z), notify the user
        if (WIFSTOPPED(status)) {
            printf("Child process was suspended. Resume it with `fg`.\n");
        }
    } else {
        perror("There was an error when creating a child process with fork().");
    }
}

/**
 * Turns terminal to the mode where it reacts to every keystroke.
 */
void enableRawMode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/**
 * Put the full command string into the history array.
 */
void addToHistory(char *command)
{
    if (history_count < HISTORY_SIZE)
    {
        // if there is enough space
        // then add new command to the history
        strcpy(history[history_count++], command);
    }
    else
    {
        // if entire history is occupied
        // shift every history element to create space for new command
        for (int i = 1; i < HISTORY_SIZE; i++)
        {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_SIZE - 1], command);
    }
    history_index = history_count;
}

/**
 * Get the previous/next command in the history.
 * direction = 1 -> previous (arrow up)
 * direction = -1 -> next (arrow down)
 */
void getCommandFromHistory(char *buffer, int direction)
{
    if (history_index >= 0)
    {
        // If user wants to go beyond history, nothing happens
        if (direction == 1 && history_index > 0)
        {
            history_index--;
        }
        else if (direction == -1 && history_index < history_count - 1)
        {
            history_index++;
        }

        strcpy(buffer, history[history_index]);
    }
    else
    {
        printf("Something went wrong with history! history_index=%d, history_count=%d\n", history_index, history_count);
    }
}

/**
 * This function enables raw mode to read user input character by character. It supports
 * navigation through command history using the up and down arrow keys. The input is stored
 * in the provided buffer.
 */
void readInput(char *buffer)
{
    // buffer received from arguments should be original fullCommand

    enableRawMode();
    int index = 0;
    char c;
    while (1)
    {
        c = getchar();
        if (c == '\n')
        {
            putchar(c);
            buffer[index] = '\0';
            break;
        }
        else if (c == 127 || c == '\b') // Handle backspace
        {
            if (index > 0)
            {
                buffer[--index] = '\0';
                printf("\b \b"); // Move cursor back, print space, move cursor back again
            }
        }
        else if (c == '\033') // Escape character (not physical ESC key)
        {
            c = getchar(); // Catch the first [ key
            // It's because presssing arrow up triggers [A and arrow up triggers [B
            if (c == '[')
            {
                c = getchar();
                if (c == 'A') // Arrow up
                {
                    buffer[0] = '\0';
                    index = 0;
                    printf("\33[2K\r"); // Clear the line
                    showCommandPrompt();
                    getCommandFromHistory(buffer, 1);
                    printf("%s", buffer);
                    index = strlen(buffer);
                }
                else if (c == 'B') // Arrow down
                {
                    buffer[0] = '\0';
                    index = 0;
                    printf("\33[2K\r"); // Clear the line
                    showCommandPrompt();
                    getCommandFromHistory(buffer, -1);
                    printf("%s", buffer);
                    index = strlen(buffer);
                }
            }
        }
        else
        {
            buffer[index++] = c;
            putchar(c);
        }
    }
    disableRawMode();
}
