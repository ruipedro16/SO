#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Process ID: %d\nParent Process ID: %d\n", getpid(), getppid());
    return 0;
}
