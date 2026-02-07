/*Qian Qiu, 100913856 - Operating systems lab 2 Group 7*/
#ifndef MYSHELL_H
#define MYSHELL_H
#include <limits.h>
#define MAX_LINE 1024
#define MAX_ARGS 64

int parse_command(char *line, char **args, char **inputf, char **outf, int *append, int *background);
/* calling commands */
void cmd_cd(char **args);
void cmd_clr(void);
void cmd_dir(char **args);
void cmd_environ(void);
void cmd_echo(char **args);
void cmd_help(void);
void cmd_pause(void);

/* utilities */
void print_prompt(void);
void reap_zombies(void);

#endif
