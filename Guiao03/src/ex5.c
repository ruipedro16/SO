#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!fork()) {
            execlp(argv[i], argv[i], NULL);
        }
    }

    for (int i = 1; i < argc; i++) {
        wait(NULL);
    }

    printf("All child processess have finished\n");

    return 0;
}
