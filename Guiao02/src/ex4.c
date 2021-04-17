#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    for (int i = 1; i <= 10; i++) {
        if (!fork()) {
            printf("%d -> Process ID: %d\n", i, getpid());
            _exit(i);
        }
    }
    int status;
    pid_t terminated_pid;
    while ((terminated_pid = wait(&status)) != -1) {
        if (WIFEXITED(status)) {
            printf("Process ID: %d -> Exit status: %d\n", terminated_pid, WEXITSTATUS(status));
        }
    }
    return 0;
}
