#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXBUFSIZE 1024

// Read size bytes from fd descriptor
ssize_t readln(int fd, char *line, size_t size) {
    char *buffer = malloc(MAXBUFSIZE * sizeof(char));
    ssize_t bytes_read = 0;
    ssize_t i = 0;
    while (i < size && (bytes_read = read(fd, line, size)) > 0) {
        i += bytes_read;
        if (line[i] != '\n') break;
    }
    return i;
}

int main() {
    char *buffer = malloc(MAXBUFSIZE * sizeof(char));
    ssize_t bytes_read = 0;

    while ((bytes_read = readln(STDIN_FILENO, buffer, MAXBUFSIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    free(buffer);
    return 0;
}
