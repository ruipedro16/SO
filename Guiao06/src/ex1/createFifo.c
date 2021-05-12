#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
    if (mkfifo("fifo", 0666) < 0) {
        perror("fifo");
        return -1;
    }
    return 0;
}
