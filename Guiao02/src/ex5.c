#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    for (int i = 1; i <= 10; i++) {
        if (!fork()) {
            printf("%d -> Process ID: %d\tParent Process ID: %d\n", i, getpid(), getppid());
        } else {
            int status;
            pid_t terminated_pid = wait(&status);
            if (WIFEXITED(status)) {
                printf("Process %d exited with status %d\n", terminated_pid, WEXITSTATUS(status));
            }
            _exit(0);
        }
    }
    return 0;
}
