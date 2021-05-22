#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Read size bytes from fd descriptor
ssize_t readln(int fd, char *line, size_t size) {
    ssize_t bytes_read = 0;
    ssize_t i = 0;
    while (i < size && (bytes_read = read(fd, line, 1)) > 0) {
        i += bytes_read;
        if (line[i] != '\n') break;
    }
    return i;
}

int main() {
    char c;
    ssize_t bytes_read = 0;

    while ((bytes_read = readln(STDIN_FILENO, &c, 1)) > 0) {
        write(STDOUT_FILENO, &c, sizeof(char));
    }

    return 0;
}
