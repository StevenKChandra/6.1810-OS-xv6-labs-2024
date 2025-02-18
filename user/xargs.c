/*
passes stdin from previous command in
the pipeline as arguments for another command
*/
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int countCharInWord(char **s) {
    int count = 0;
    char *p = (char *) *s;
    while (*p != ' ' && *p != '\n' && *p != '\0') {
        count++;
        p++;
    }
    return count;
}

void parseWord(char *argv, char **p) {
    while (**p != ' ' && **p != '\n' && **p != '\0') {
        *argv++ = *(*p)++;
    }
    *argv = '\0';
}

void parseArgv(char **argv, char **p, int *argc) {
    while (**p != '\0' && **p != '\n') {
        if (*argc >= 32) {
            printf("too many arguments\n");
            break;
        }

        int count = countCharInWord(p);
        if (count == 0) {
            *p += 1;
            continue;
        }

        if (count > 63) {
            printf("one of the argument is too long\n");
            *p += count + 1;
            continue;
        }
        argv[*argc] = malloc(count * sizeof(char) + 1);
        parseWord(argv[(*argc)++], p);
    }
    if (**p != '\0') *p += 1;
}

int
main(int argc, char *argv[]) {
    char buf[128], *p;
    char *newArgv[32];

    for (int i = 1; i < argc; i++) {
        newArgv[i-1] = argv[i];
    }

    p = buf;
    if(read(0, buf, sizeof(buf)) > 0) {
        while (*p != '\0') {
            int i = argc - 1;
            parseArgv(newArgv, &p, &i);
            if (fork() == 0) {
                exec(newArgv[0], newArgv);
            }
            wait(0);
        }
    }
    exit(0);
}