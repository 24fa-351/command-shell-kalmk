#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

int main()
{
    char input[MAX_INPUT];
    char *tokens[MAX_TOKENS];
    int background, input_redir, output_redir;
    char *input_file = NULL, *output_file = NULL;

    while (1)
    {
        printf("xsh# ");
        if (!fgets(input, sizeof(input), stdin))
        {
            break;
        }

        parse_input(input, tokens);

        if (tokens[0] &&
            (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0))
        {
            break;
        }

        if (tokens[0] && strcmp(tokens[0], "cd") == 0)
        {
            handle_cd(tokens);
            continue;
        }

        if (tokens[0] && strcmp(tokens[0], "pwd") == 0)
        {
            handle_pwd();
            continue;
        }

        if (tokens[0] && strcmp(tokens[0], "set") == 0)
        {
            handle_set(tokens);
            continue;
        }

        if (tokens[0] && strcmp(tokens[0], "unset") == 0)
        {
            handle_unset(tokens);
            continue;
        }

        if (tokens[0] && strcmp(tokens[0], "echo") == 0)
        {
            handle_echo(tokens);
            continue;
        }

        background = 0;
        input_redir = 0;
        output_redir = 0;
        input_file = output_file = NULL;

        if (tokens[0] && strcmp(tokens[0], "&") == 0)
        {
            background = 1;
            tokens[0] = NULL;
        }

        for (int i = 0; tokens[i] != NULL; i++)
        {
            if (strcmp(tokens[i], "<") == 0)
            {
                input_redir = 1;
                input_file = tokens[i + 1];
                tokens[i] = NULL;
                break;
            }
        }

        for (int ix = 0; tokens[ix] != NULL; ix++)
        {
            if (strcmp(tokens[ix], ">") == 0)
            {
                output_redir = 1;
                output_file = tokens[ix + 1];
                tokens[ix] = NULL;
                break;
            }
        }

        char *pipe_tokens[MAX_TOKENS];
        int pipe_index = 0;
        for (int ix = 0; tokens[ix] != NULL; ix++)
        {
            if (strcmp(tokens[ix], "|") == 0)
            {
                pipe_tokens[pipe_index] = NULL;
                pipe_index++;
                continue;
            }
            pipe_tokens[pipe_index++] = tokens[ix];
        }
        pipe_tokens[pipe_index] = NULL;

        if (pipe_index > 0)
        {
            execute_command(pipe_tokens, background, input_redir, output_redir,
                            input_file, output_file);
        }
        else
        {
            execute_command(tokens, background, input_redir, output_redir,
                            input_file, output_file);
        }
    }
    return 0;
}