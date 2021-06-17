#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXBUFSIZE 1024
#define STRLEN 128
#define SERVER_FIFO "tmp/fifo_server"
#define STATUS_FIFO "tmp/status"

#define WRITE_LITERAL(fd, str) write(fd, str, sizeof(str))

typedef struct {
    char *input_file;
    char *output_file;
    char filters[STRLEN];
} Request;

void handle_request(const char *pipe, Request *request) {
    int fd;
    if ((fd = open(pipe, O_RDONLY)) < 0) {
        perror("Error opening client pipe");
        _exit(-1);
    }

    int output_fd;
    if ((output_fd = open(request->output_file, O_WRONLY | O_CREAT, 0666)) < 0) {
        perror("Error opening output file");
        _exit(-1);
    }

    char buffer[MAXBUFSIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, MAXBUFSIZE)) > 0) {
        if (strcmp(buffer, "Invalid request") && strcmp(buffer, "Request Discarded")) {
            write(output_fd, buffer, bytes_read);
        }
    }

    close(output_fd);
    unlink(pipe);
}

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        WRITE_LITERAL(STDOUT_FILENO, "USAGE:\n");
        WRITE_LITERAL(STDOUT_FILENO, "./aurras status\n");
        WRITE_LITERAL(STDOUT_FILENO,
                      "./aurras transform input-filename output-filename filter "
                      "id-1 filter id-2 ...\n");
    } else {
        int server_fd;
        if ((server_fd = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK)) < 0) {
            perror("Error opening server pipe");
            return -1;
        }

        if (argc == 2 && !strcmp(argv[1], "status")) {
            WRITE_LITERAL(server_fd, "status");
            int status_fd;
            if ((status_fd = open(STATUS_FIFO, O_RDONLY)) < 0) {
                perror("Error opening status pipe");
                return -1;
            }
            char buffer[MAXBUFSIZE];
            ssize_t bytes_read;
            while ((bytes_read = read(status_fd, buffer, 1)) > 0) {
                write(STDOUT_FILENO, buffer, bytes_read);
            }
            close(status_fd);
        } else if (argc > 2 && !strcmp(argv[1], "transform")) {
            char str[MAXBUFSIZE];
            sprintf(str, "tmp/%d", getpid());
            if (mkfifo(str, 0666) < 0) {
                perror("Error creating fifo");
                return -1;
            }
            Request request;
            request.input_file = strdup(argv[2]);
            request.output_file = strdup(argv[3]);
            memset(request.filters, 0, sizeof(request.filters));

            for (unsigned int i = 4; i < argc; i++) {
                strcat(request.filters, argv[i]);
                if (i + 1 < argc) {
                    strcat(request.filters, " ");
                }
            }
            char buffer[MAXBUFSIZE];
            sprintf(buffer, "%d %s %s %s\n", getpid(), request.input_file, request.output_file,
                    request.filters);
            write(server_fd, buffer, strlen(buffer));
            handle_request(str, &request);
        } else {
            perror("Insufficient number of arguments");
            return -1;
        }

        return 0;
    }
}
