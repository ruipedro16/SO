
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define MAX 1024

int main() {
    int fd;
    if ((fd = open("fifo", O_WRONLY)) < 0) {
        perror("open");
        return -1;
    }

    char buffer[MAX];
    ssize_t size;

    while ((size = read(STDIN_FILENO, buffer, 1024)) > 0) {
        write(fd, buffer, size);
    }

    close(fd);

    return 0;
}
