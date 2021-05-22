#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int spawn_grep(const char *word, const char *file, int i) {
    pid_t pid;
    if (!(pid = fork())) {
        printf("process %d with pid %d exec(grep) for file %s\n", i, getpid(), file);

        if (execlp("grep", "grep", word, file, NULL) < 0) {
            perror("exec failed");
        }
        _exit(-1);
    }
    return pid;
}

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        puts("Usage: multigrep <word> file1 file2 ...");
        return -1;
    }

    int files_count = argc - 2;
    int pids[files_count];
    char **files = (char **)argv + 2;

    for (size_t i = 0; i < files_count; i++) {
        pids[i] = spawn_grep(argv[1], files[i], i);
    }

    int found = 0;
    int status = 0;
    int pid;
    int pid_found = -1;

    while (!found && (pid = wait(&status)) > 0) {
        // grep return 0 if anything was found
        if (WIFEXITED(status)) {
            switch (WEXITSTATUS(status)) {
                case 0:
                    printf("process %d found the word\n", pid);
                    found = 1;
                    pid_found = pid;
                    break;
                case 1:
                    printf("process %d did not find the word\n", pid);
                    break;
            }
        }
    }

    if (found) {
        for (size_t i = 0; i < files_count; i++) {
            if (pids[i] != pid_found) {
                printf("killing process %d\n", pids[i]);

                // evitar a possibilidade de um kill -1
                if (pids[i] > 0) {
                    kill(pids[i], SIGKILL);
                }

                // mostra que processo foi interrompido
                waitpid(pids[i], &status, 0);
                if (WIFEXITED(status)) {
                    printf("process %d ended correctly already\n", pids[i]);
                } else {
                    printf("process %d was interrupted\n", pids[i]);
                }
            }
        }

        return 0;
    }
}
