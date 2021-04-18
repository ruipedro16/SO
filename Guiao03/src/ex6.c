#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 20

int my_system(const char *command) {
    char *args[MAX_ARGS];
    char *cmd = strdup(command);
    char *str = strtok(cmd, " ");

    size_t i;
    for (i = 0; str; i++) {
        args[i] = str;
        str = strtok(NULL, " ");
    }
    args[i] = NULL;

    if (!fork()) {
        int ret = execvp(args[0], args);
        _exit(ret);
    } else {
        wait(NULL);
    }

    if (cmd) free(cmd);

    return 0;
}

int main() {
    const char *commands[] = {"ls -l", "sleep 1", "ps", "cat ex155.c"};

    for (size_t i = 0; i < 4; i++) {
        printf("command: %s\n", commands[i]);
        int ret = my_system(commands[i]);
        printf("ret = %d\n\n", ret);
    }

    return 0;
}
