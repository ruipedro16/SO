#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX 1024

int main() {
    if (mkfifo("fifo", 0666) < 0) {
        if (errno != EEXIST) {
            perror("fifo");
            return -1;
        }
    }

    int log_fd;
    if ((log_fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
        perror("log_open");
        return -1;
    }

    char buffer[MAX];
    ssize_t size;
    int fd;

    time_t current_time;
    char timestamp[20];
    struct tm *tm_info;

    for (;;) {
        if ((fd = open("fifo", O_RDONLY)) < 0) {
            perror("open");
            return -1;
        }

        while ((size = read(fd, buffer, 1024)) > 0) {
            current_time = time(NULL);
            tm_info = localtime(&current_time);
            strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S ", tm_info);
            write(log_fd, timestamp, sizeof(timestamp));
            write(log_fd, buffer, size);
            write(log_fd, "\n", sizeof("\n"));
        }
    }

    close(log_fd);
    close(fd);

    return 0;
}
