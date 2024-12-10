#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

void parse_input(char *input, char **tokens)
{
    char *token = strtok(input, " \n\t");
    int ix = 0;
    while (token != NULL)
    {
        tokens[ix++] = token;
        token = strtok(NULL, " \n\t");
    }
    tokens[ix] = NULL;
}

void handle_cd(char **tokens)
{
    if (tokens[1] == NULL)
    {
        fprintf(stderr, "cd: expected argument\n");
    }
    else if (chdir(tokens[1]) != 0)
    {
        perror("cd");
    }
}

void handle_pwd()
{
    char cwd[MAX_INPUT];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("pwd");
    }
}

void handle_set(char **tokens)
{
    if (tokens[1] != NULL && tokens[2] != NULL)
    {
        if (setenv(tokens[1], tokens[2], 1) != 0)
        {
            perror("setenv");
        }
    }
    else
    {
        fprintf(stderr, "Usage: set <VAR> <VALUE>\n");
    }
}

void handle_unset(char **tokens)
{
    if (tokens[1] != NULL)
    {
        if (unsetenv(tokens[1]) != 0)
        {
            perror("unsetenv");
        }
    }
    else
    {
        fprintf(stderr, "Usage: unset <VAR>\n");
    }
}

void handle_echo(char **tokens)
{
    for (int ix = 1; tokens[ix] != NULL; ix++)
    {
        if (tokens[ix][0] == '$')
        {
            char *env_val = getenv(tokens[ix] + 1);
            if (env_val)
            {
                printf("%s ", env_val);
            }
            else
            {
                printf(" (undefined) ");
            }
        }
        else
        {
            printf("%s ", tokens[ix]);
        }
    }
    printf("\n");
}

void execute_command(char **tokens, int background, int input_redir,
                     int output_redir, char *input_file, char *output_file)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        if (input_redir)
        {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd == -1)
            {
                perror("Error opening input file");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_redir)
        {
            int output_fd =
                open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1)
            {
                perror("Error opening output file");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        if (execvp(tokens[0], tokens) == -1)
        {
            perror("Execution failed");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        if (!background)
        {
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        perror("Fork failed");
    }
}