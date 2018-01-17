/**
* Name : Sachin G. Bagalakoti
* Date : 13-01-2018
* 
**/
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 1024
#define ARGBUFFER 64
#define DELIM " \t\r\n\a"

int cd(char **args);
int help(char **args);
int Exit(char **args);

char *str[] = {
    "cd","help","exit"
};

int (*function[]) (char **) = {
    &cd,&help,&exit
};

int totalFunc() {
    return sizeof(str) / sizeof(char *);
}

int cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "sh: expected argument to \"cd\"\n");
    }
    else {
        /// if chdir returns 0 then it successfully executed command.
        if (chdir(args[1]) != 0) {
            perror("sh");
        }
    }
    return 1;
}

int help(char **args)
{
    int i;
    printf("The following are built in Functions :\n");

    for (i = 0; i < totalFunc(); i++) {
        printf("  %s\n", str[i]);
    }
    printf("Type the command to know more about it.");
    return 1;
}

int Exit(char **args)
{
    return 0;
}

int launch(char **args)
{
    pid_t pid;     /// fork() returns pit_t type, which is kinda int.
    int status;

    pid = fork(); /// it creates two processes.
    if (pid == 0) {
        ///the execution of command is done here, over childs processor (which was created by fork() method.
        /// exec command with 'v'ector(args) and 'p'rogram name(program path).
        if (execvp(args[0], args) == -1) {
            perror("sh");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("sh");
    }
    else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        /// we use the macros provided with waitpid() to wait until either the processes are exited(WIFEXITED) or killed(WIFSIGNALED).
    }

    return 1;
}

int execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < totalFunc(); i++) {
        if (strcmp(args[0], str[i]) == 0) {
            return (*function[i])(args);
        }
    }

    return launch(args);
}

char **split_input(char *line)
{
    int bufferSize = ARGBUFFER, position = 0;
    char **tokens = malloc(bufferSize * sizeof(char*));
    char *token, **backup;

    if (!tokens) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufferSize) {
            bufferSize += ARGBUFFER;
            backup = tokens;
            tokens = realloc(tokens, bufferSize * sizeof(char*));
            if (!tokens) {
                free(backup);
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

char *read_input(void)
{
    int bufferSize = BUFFERSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufferSize);
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        }
        else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufferSize) {
            bufferSize += BUFFERSIZE;
            buffer = realloc(buffer, bufferSize);

            if (!buffer) {
                fprintf(stderr, "sh: allocation error\n");
            exit(EXIT_FAILURE);
            }
        }
    }
}

int main(int argc, char **argv)
{
    char *input;    /// TO read input command from keyboard.
    char **args;    /// holds all tokens of input command
    int status;     /// return status

    printf("********************* WELCOME TO MYSHELL *********************\n");
    do {
        printf(">> ");
        input = read_input();
        args = split_input(input);
        status = execute(args);

        free(input);
        free(args);
    } while (status);


    return EXIT_SUCCESS;
}


