#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    for (int i = 1; i <= 10; i++) {
        if (!fork()) {
            printf("%d -> Process ID: %d\t Parent Process ID: %d\n", i, getpid(), getppid());
            _exit(i);
        } else {
            int status;
            pid_t terminated_pid = wait(&status);
            if (WIFEXITED(status)) {
                printf("Process %d terminated with status %d\n\n", terminated_pid,
                       WEXITSTATUS(status));
            }
        }
    }
    return 0;
}
