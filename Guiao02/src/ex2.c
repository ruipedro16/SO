#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid;
    if (!(pid = fork())) {
        printf("Child Process\nProcess ID: %d\tParent Process ID: %d\n\n", getpid(), getppid());
    } else {
        printf(
            "Parent Process\nProcess ID: %d\tChild Process ID: %d\tParent "
            "Process ID: %d\n\n",
            getpid(), pid, getppid());
    }
    return 0;
}
