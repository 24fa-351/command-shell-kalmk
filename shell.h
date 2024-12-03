#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 1024
#define MAX_TOKENS 100

void parse_input(char *input, char **tokens);
void handle_cd(char **tokens);
void handle_pwd();
void handle_set(char **tokens);
void handle_unset(char **tokens);
void handle_echo(char **tokens);
void execute_command(char **tokens, int background, int input_redir,
                     int output_redir, char *input_file, char *output_file);

#endif // SHELL_H