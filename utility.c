#define _POSIX_C_SOURCE 200809L
#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

extern char **environ;

void reap_zombies(void) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void print_prompt(void) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s> ", cwd);
    fflush(stdout);
}

/* command functions */
void cmd_cd(char **args) {
    if (!args[1]) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        return;
    }
    if (chdir(args[1]) != 0)
        perror("cd");
}

void cmd_clr(void) {
    printf("\033[H\033[J");
}

void cmd_dir(char **args) {
    char *path = args[1] ? args[1] : ".";
    DIR *d = opendir(path);
    if (!d) {
        perror("dir");
        return;
    }
    struct dirent *e;
    while ((e = readdir(d)) != NULL)
        printf("%s\n", e->d_name);
    closedir(d);
}

void cmd_environ(void) {
    for (char **e = environ; *e; e++)
        printf("%s\n", *e);
}

void cmd_echo(char **args) {
    for (int i = 1; args[i]; i++)
        printf("%s ", args[i]);
    printf("\n");
}

void cmd_help(void) {
    system("more readme");
}

void cmd_pause(void) {
    printf("Press ENTER to continue...");
    getchar();
}

