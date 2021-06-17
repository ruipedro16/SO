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
#define MAX_FILTERS 10
#define MAX_REQUESTS 20
#define READ_END 0
#define WRITE_END 1
#define SERVER_FIFO "tmp/fifo_server"
#define STATUS_FIFO "tmp/status"

#define WRITE_LITERAL(fd, str) write(fd, str, sizeof(str))

typedef struct {
    char *id;
    char *exec;  // executable
    unsigned int total;
    unsigned int available;
} Filter;

typedef struct {
    unsigned int id;
    pid_t pid;
    char *input_file;
    char *output_file;
    char filters[STRLEN];
} Request;

// Global variables
pid_t server_pid;
pid_t requests_pid[MAX_REQUESTS];
char *filters_folder;
unsigned int n_filters = 0;
Filter filters[MAX_FILTERS];
unsigned int total = 0;
Request current_request;
char *applied_filters[MAX_REQUESTS];  // recover filters

int filter_pipe[2];

ssize_t readln(int fd, char *buf, size_t nbyte) {
    ssize_t ret = 0;

    if (nbyte == 0) {
        goto RET;
    }

    char const *const end = buf + nbyte - 1;
    char *i = buf;
    for (; i != end; ++i) {
        ssize_t const n = read(fd, i, 1);
        if (n == -1) {
            // if read fails, we discard the whole line.
            *buf = '\0';
            ret = -1;
            goto RET;
        }
        if (n == 0 || *i == '\n') {
            break;
        }
    }

    ret = i - buf;
    *i = '\0';

RET:
    return ret;
}

void parse_filter(char *line, size_t index) {
    char *c;
    unsigned int i;
    Filter *filter = malloc(sizeof(Filter));
    for (i = 0, c = strtok(line, " "); c; i++, c = strtok(NULL, " ")) {
        switch (i) {
            case 0:
                filter->id = strdup(c);
                break;
            case 1:
                filter->exec = strdup(c);
                break;
            case 2:
                filter->available = atoi(c);
                filter->total = atoi(c);
                filters[index] = *filter;
                n_filters++;
        }
    }
}

void parse_config(const char *filepath) {
    int fd;
    if ((fd = open(filepath, O_RDONLY)) < 0) {
        perror("Error opening config file");
        _exit(-1);
    }

    char buffer[MAXBUFSIZE];
    unsigned int i = 0;
    while (readln(fd, buffer, MAXBUFSIZE) > 0) {
        parse_filter(buffer, i);
        i++;
    }
    close(fd);
    char response[STRLEN];
    sprintf(response, "Config file %s parsed\t %d filters\n\n", filepath, n_filters);
    write(STDOUT_FILENO, response, strlen(response));
}

void apply_filters(unsigned int num_filters, char **filters, Request *request) {
    sleep(5);
    char tmp[MAXBUFSIZE];
    sprintf(tmp, "tmp/%d", request->pid);

    int fd_in, fd_out;

    if ((fd_in = open(request->input_file, O_RDONLY)) < 0) {
        perror("Error opening input file\n");
        _exit(-1);
    }

    if ((fd_out = open(tmp, O_WRONLY)) < 0) {
        perror("Error opening file");
        _exit(-1);
    }

    int fd[2];
    for (unsigned int i = 0; i < num_filters; i++) {
        if (pipe(fd) < 0) {
            perror("Error creating pipe");
            return;
        }

        char cmd[MAXBUFSIZE];
        memset(cmd, 0, sizeof(cmd));
        strcat(cmd, filters_folder);
        strcat(cmd, filters[i]);

        pid_t pid;
        if ((pid = fork()) < 0) {
            perror("Fork failed");
            return;
        } else if (!pid) {
            if (i == 0) {  // first filter
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            close(fd[READ_END]);

            if (i == num_filters - 1) {  // last filter
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
                if (execl(cmd, cmd, NULL) == -1) {
                    perror("Exec failed");
                    _exit(-1);
                }
            } else {
                dup2(fd[WRITE_END], STDOUT_FILENO);
                close(fd[WRITE_END]);
                if (execl(cmd, cmd, NULL) == -1) {
                    perror("Exec failed");
                    _exit(-1);
                }
            }
        } else {
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[WRITE_END]);
            close(fd[READ_END]);
        }
    }
}

void process_request(Request *request) {
    unsigned int num_filters = 0;
    char *filters_exec[n_filters];                 // executables
    char *req_filters = strdup(request->filters);  // required filters

    for (char *c = strtok(req_filters, " "); c; c = strtok(NULL, " ")) {
        for (unsigned int i = 0; i < n_filters; i++) {
            if (!strcmp(filters[i].id, c) && !filters[i].available) {
                WRITE_LITERAL(STDERR_FILENO, "Request Discarded\n");
                free(req_filters);
                return;
            }
        }
    }
    free(req_filters);

    char *filters_aux = strdup(request->filters);
    for (char *c = strtok(filters_aux, " "); c; c = strtok(NULL, " ")) {
        for (unsigned int i = 0; i < n_filters; i++) {
            if (!strcmp(filters[i].id, c)) {
                filters[i].available--;
                filters_exec[num_filters] = filters[i].exec;
                num_filters++;
                break;
            }
        }
    }

    // Here, all filters are available
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("Fork failed");
        return;
    } else if (!pid) {
        apply_filters(num_filters, filters_exec, request);
        _exit(0);
    } else {
        char tmp[MAXBUFSIZE];
        sprintf(tmp, "%d %s", pid, request->filters);

        for (unsigned int i = 0; i < MAX_REQUESTS; i++) {
            if (!applied_filters[i]) {
                applied_filters[i] = tmp;
                break;
            }
        }

        for (unsigned int i = 0; i < MAX_REQUESTS; i++) {
            if (requests_pid[i] == 0) {
                requests_pid[i] = pid;
                break;
            }
        }
    }
}

void handle_request(const char *buffer) {
    char *_buffer = strdup(buffer);
    _buffer = strtok(_buffer, "\n");
    unsigned int i;
    char *c;
    memset(current_request.filters, 0, sizeof(current_request.filters));
    for (i = 0, c = strtok(_buffer, " "); c; i++, c = strtok(NULL, " ")) {
        if (!i) {
            current_request.pid = atoi(c);
        } else if (i == 1) {
            current_request.input_file = strdup(c);
        } else if (i == 2) {
            current_request.output_file = strdup(c);
        } else {
            strcat(current_request.filters, c);
            strcat(current_request.filters, " ");
        }
    }

    if (!strcmp(current_request.filters, "")) {
        strcpy(current_request.filters, "N/A");
    }

    char response[2 * STRLEN];
    sprintf(response, "PID: %d\n", current_request.pid);
    write(STDERR_FILENO, response, strlen(response));
    sprintf(response, "Input: %s\nOutput: %s\n", current_request.input_file,
            current_request.output_file);
    write(STDERR_FILENO, response, strlen(response));
    sprintf(response, "Filter: %s\n", current_request.filters);
    write(STDERR_FILENO, response, strlen(response));

    if (i <= 3) {
        sprintf(response, "Request #%d is invalid\n\n", current_request.pid);
        write(STDERR_FILENO, response, strlen(response));
        char tmp[MAXBUFSIZE];
        int fd;
        sprintf(tmp, "tmp/%d", current_request.pid);
        if ((fd = open(tmp, O_WRONLY)) < 0) {
            perror("Error opening file");
            _exit(-1);
        }
        WRITE_LITERAL(fd, "Invalid request");
        close(fd);
    } else {
        WRITE_LITERAL(STDOUT_FILENO, "\n");
        current_request.id = ++total;
        process_request(&current_request);
    }
}

void update_filters(pid_t child_pid) {
    for (unsigned int i = 0; i < MAX_REQUESTS; i++) {
        if (requests_pid[i] == child_pid) {
            requests_pid[i] = 0;
            for (int j = 0; j < MAX_REQUESTS; j++) {
                if (applied_filters[j]) {
                    char *buffer = strdup(applied_filters[j]);
                    char *c;
                    unsigned int i;
                    for (i = 0, c = strtok(buffer, " "); c; i++, c = strtok(NULL, " ")) {
                        if (!i) {
                            if (atoi(c) != child_pid) {
                                break;
                            }
                        } else {
                            for (unsigned int k = 0; k < MAX_FILTERS; k++) {
                                if (!strcmp(filters[k].id, c)) {
                                    filters[k].available++;
                                    break;
                                }
                            }
                        }
                    }
                    if (i > 0) {
                        applied_filters[j] = NULL;
                        break;
                    }
                }
            }
        }
    }
}

void send_status() {
    char response[MAXBUFSIZE] = {0};
    int fd;
    if ((fd = open(STATUS_FIFO, O_WRONLY)) < 0) {
        perror("Error opening status pipe");
        _exit(-1);
    }
    pid_t pid = getpid();
    if (pid == server_pid) {
        for (unsigned int i = 0; i < n_filters; i++) {
            if (strlen(filters[i].id) > 0) {
                char tmp[MAXBUFSIZE];
                sprintf(tmp, "Filter %s: %d/%d\n", filters[i].id, filters[i].available,
                        filters[i].total);
                strcat(response, tmp);
            }
        }
    } else {
        sprintf(response, "Task #%d: transform %s %s %s\n", current_request.id,
                current_request.input_file, current_request.output_file, current_request.filters);
    }
    write(fd, response, strlen(response));
    close(fd);
}

void sigterm_handler(int signum) {
    WRITE_LITERAL(STDOUT_FILENO, "Received SIGTERM signal\n");
    if (server_pid == getpid()) {
        for (;;) {
            int waiting = 0;
            for (unsigned int i = 0; i < MAX_REQUESTS; i++) {
                if (requests_pid[i]) waiting++;
            }
            if (!waiting) {
                break;
            }
        }
        unlink(SERVER_FIFO);
        unlink(STATUS_FIFO);
        exit(0);
    }
}

void sigchld_handler(int signum) {
    int status;
    pid_t child = wait(&status);
    if (WIFEXITED(status) && server_pid == getpid()) {
        update_filters(child);
    }
}

void sigusr_handler(int signum) { send_status(); }

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        WRITE_LITERAL(STDOUT_FILENO, "USAGE:\n");
        WRITE_LITERAL(STDOUT_FILENO, "./aurrasd config-filename filters-folder\n");
        return 0;
    } else {
        char buff[STRLEN];
        sprintf(buff, "Server Staring \t (PID = %d)\n", getpid());
        write(STDOUT_FILENO, buff, strlen(buff));

        if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
            perror("SIGTERM failed");
            return -1;
        }

        if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
            perror("SIGCHLD failed");
            return -1;
        }

        if (signal(SIGUSR1, sigusr_handler) == SIG_ERR) {
            perror("SIGUSR failed");
            return -1;
        }

        if (mkfifo(SERVER_FIFO, 0666) < 0) {
            perror("Error creating server pipe");
            return -1;
        }

        if (mkfifo(STATUS_FIFO, 0666) < 0) {
            perror("Error creating status pipe");
            return -1;
        }

        server_pid = getpid();
        filters_folder = strdup(argv[2]);
        parse_config(argv[1]);

        int fd;
        if ((fd = open(SERVER_FIFO, O_RDONLY)) < 0) {
            perror("Error opening server pipe");
            return -1;
        }

        if (pipe(filter_pipe) < 0) {
            perror("Error opening pipe");
            return -1;
        }

        for (;;) {
            char buffer[MAXBUFSIZE] = {0};
            ssize_t bytes_read;
            while ((bytes_read = read(fd, buffer, MAXBUFSIZE)) > 0) {
                if (!strcmp(buffer, "status")) {
                    for (unsigned int i = 0; i < MAX_REQUESTS; i++) {
                        if (requests_pid[i]) {
                            kill(requests_pid[i], SIGUSR1);
                        }
                    }
                    kill(server_pid, SIGUSR1);
                } else {
                    WRITE_LITERAL(STDOUT_FILENO, "Request received: ");
                    write(STDOUT_FILENO, buffer, bytes_read);
                    WRITE_LITERAL(STDOUT_FILENO, "\n");
                    handle_request(buffer);
                }
            }
        }
        return 0;
    }
}
