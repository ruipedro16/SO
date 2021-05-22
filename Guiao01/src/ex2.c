#include <stdlib.h>
#include <unistd.h>

#define MAXBUFSIZE 1024

int main(int argc, char const *argv[]) {
    char *buffer = malloc(MAXBUFSIZE * sizeof(char));
    ssize_t bytes_read = 0;

    while ((bytes_read = read(STDIN_FILENO, buffer, MAXBUFSIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    return 0;
}
