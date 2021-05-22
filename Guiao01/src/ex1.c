#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define MAXBUFSIZE 1024

int main(int argc, char const *argv[]) {
    int fd_orig, fd_dest;

    if ((fd_orig = open(argv[1], O_RDONLY)) < 0) {
        perror("open");
        return -1;
    }
    if ((fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0640)) < 0) {
        perror("open");
        return -1;
    }

    char buffer[MAXBUFSIZE];
    ssize_t bytes_read = 0;

    while ((bytes_read = read(fd_orig, buffer, MAXBUFSIZE)) > 0) {
        write(fd_dest, buffer, bytes_read);
    }
    close(fd_orig);
    close(fd_dest);

    return 0;
}
