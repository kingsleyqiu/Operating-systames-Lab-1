#define _POSIX_C_SOURCE 200809L

#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/* ---------- Parsing ---------- */
int parse_command(char *line, char **args, char **infile, char **outfile, int *append, int *background) {

    *infile = *outfile = NULL;
    *append = *background = 0;

    line[strcspn(line, "\n")] = 0;

    int i = 0;
    char *tok = strtok(line, " ");
    while (tok) {
        if (!strcmp(tok, "<")) {
            *infile = strtok(NULL, " ");
        } else if (!strcmp(tok, ">")) {
            *outfile = strtok(NULL, " ");
            *append = 0;
        } else if (!strcmp(tok, ">>")) {
            *outfile = strtok(NULL, " ");
            *append = 1;
        } else if (!strcmp(tok, "&")) {
            *background = 1;
        } else {
            args[i++] = tok;
        }
        tok = strtok(NULL, " ");
    }
    args[i] = NULL;
    return i;
}

/* ---------- Main ---------- */
int main(int argc, char *argv[]) {
    FILE *input = stdin;
    char line[MAX_LINE];

    /* Required by lab */
    setenv("shell", argv[0], 1);

    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("batch file");
            exit(1);
        }
    }

    while (1) {
        if (input == stdin)
            print_prompt();

        if (!fgets(line, sizeof(line), input))
            break;

        char *args[MAX_ARGS];
        char *infile, *outfile;
        int append, background;

        if (!parse_command(line, args, &infile, &outfile, &append, &background))
            continue;

        if (!strcmp(args[0], "quit"))
            break;

        int saved_in = dup(0), saved_out = dup(1);

        if (infile) {
            int fd = open(infile, O_RDONLY);
            dup2(fd, 0);
            close(fd);
        }
        if (outfile) {
            int fd = open(outfile,
                O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC),
                0644);
            dup2(fd, 1);
            close(fd);
        }

        if (!strcmp(args[0], "cd")) cmd_cd(args);
        else if (!strcmp(args[0], "clr")) cmd_clr();
        else if (!strcmp(args[0], "dir")) cmd_dir(args);
        else if (!strcmp(args[0], "environ")) cmd_environ();
        else if (!strcmp(args[0], "echo")) cmd_echo(args);
        else if (!strcmp(args[0], "help")) cmd_help();
        else if (!strcmp(args[0], "pause")) cmd_pause();
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

    if (input != stdin)
        fclose(input);

    return 0;
}

