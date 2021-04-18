#include <unistd.h>

int main() {
    char *const args[] = {"argument 0", "argument 1", "argument 2", NULL};
    execvp("./ex3", args);
    return 0;
}
