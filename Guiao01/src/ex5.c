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
    unsigned int line_no;
    char *buffer = malloc(MAXBUFSIZE * sizeof(char));
    ssize_t bytes_read = 0;

    for (line_no = 1; (bytes_read = readln(STDIN_FILENO, buffer, MAXBUFSIZE)) > 0; line_no++) {
        char line_number[10] = "";
        sprintf(line_number, "%d: ", line_no);
        write(STDOUT_FILENO, line_number, sizeof(line_number));
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    free(buffer);
    return 0;
}
