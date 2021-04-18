#include <stdio.h>
#include <unistd.h>

int main() {
    // execl("/bin/ls", "ls", "-l", NULL);
    // execlp("ls", "ls", "-l", NULL);
    char *const args[] = {"ls", "-l", NULL};
    // execv("/bin/ls", args);
    execvp("ls", args);
    printf("Done");
    return 0;
}
