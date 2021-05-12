#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) return -1;

    int fd;
    if ((fd = open("fifo", O_WRONLY)) < 0) {
        perror("open");
        return -1;
    }

    if (write(fd, argv[1], strlen(argv[1])) < 0) {
        perror("write");
        return -1;
    }

    close(fd);

    return 0;
}
