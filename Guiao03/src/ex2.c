#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    if (!fork()) {
        printf("SON: %d\n", getpid());
        char *const args[] = {"ls", "-l", NULL};
        execvp("ls", args);
    } else {
        printf("FATHER: %d\n", getpid());
        int status;
        pid_t terminated_pid = wait(&status);
        WIFEXITED(status) ? printf("Child process %d terminated with status %d\n", terminated_pid,
                                   WEXITSTATUS(status))
                          : printf("Error\n");
    }
    return 0;
}
