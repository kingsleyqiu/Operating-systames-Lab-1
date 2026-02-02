#define _POSIX_C_SOURCE 200809L

#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/*This is used so that a command can be broken into small parts that can then be read*/
int parse_command(char *line, char **args, char **inputf, char **outf, int *append, int *background) {
    *inputf = *outf = NULL;
    *append = *background = 0;
    line[strcspn(line, "\n")] = 0;
    int i = 0;
    char *x = strtok(line, " ");

    while (x) {
        if (!strcmp(x, "<")) {
            *inputf = strtok(NULL, " ");
        } else if (!strcmp(x, ">")) {
            *outf = strtok(NULL, " ");
            *append = 0;
        } else if (!strcmp(x, ">>")) {
            *outf = strtok(NULL, " ");
            *append = 1;
        } else if (!strcmp(x, "&")) {
            *background = 1;
        } else {
            args[i++] = x;
        }
        x = strtok(NULL, " ");
    }
    args[i] = NULL;
    return i;
}

/* main functions */
int main(int argc, char *argv[]) {
    FILE *in = stdin;
    char line[MAX_LINE];
    setenv("shell", argv[0], 1);

    /* if an error occured */
    if (argc == 2) {
        in = fopen(argv[1], "r");
        if (!in) {
            perror("batch file");
            exit(1);
        }
    }

    /* if normal then excute this loop */
    while (1) {
        if (in == stdin) {
            print_prompt();
        }
        if (!fgets(line, sizeof(line), in)) {
            break;
        }

        char *args[MAX_ARGS];
        char *inputf, *outf;
        int append, background;

        if (!parse_command(line, args, &inputf, &outf, &append, &background)) {
            continue;
        }
        if (!strcmp(args[0], "quit")) {
            break;
        }

        int saved_in = dup(0), saved_out = dup(1);

        if (inputf) {
            int fd = open(inputf, O_RDONLY);
            dup2(fd, 0);
            close(fd);
        }
        if (outf) {
            int fd = open(outf, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            dup2(fd, 1);
            close(fd);
        }

        if (!strcmp(args[0], "cd")) {
            cmd_cd(args);
        }
        else if (!strcmp(args[0], "clr")) {
            cmd_clr();
        }
        else if (!strcmp(args[0], "dir")) {
            cmd_dir(args);
        }
        else if (!strcmp(args[0], "environ")) {
            cmd_environ();
        }
        else if (!strcmp(args[0], "echo")) {
            cmd_echo(args);
        }
        else if (!strcmp(args[0], "help")) {
            cmd_help();
        }
        else if (!strcmp(args[0], "pause")) {
            cmd_pause();
        }
        else {
            pid_t pid = fork();
            if (pid == 0) {
                setenv("parent", getenv("shell"), 1);
                execvp(args[0], args);
                perror("exec");
                exit(1);
            } else if (!background) {
                waitpid(pid, NULL, 0);
            }
        }

        dup2(saved_in, 0);
        dup2(saved_out, 1);
        close(saved_in);
        close(saved_out);
        reap_zombies();
    }

    if (in != stdin) {
        fclose(in);
    }
    
    return 0;
}

