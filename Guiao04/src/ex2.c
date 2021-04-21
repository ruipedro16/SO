#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int input_fd = open("/etc/passwd", O_RDONLY);
    int output_fd = open("saida.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int error_fd = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    int stdout_fd_backup = dup(STDOUT_FILENO);
    int stdin_fd = dup2(input_fd, STDIN_FILENO);
    int stdout_fd = dup2(output_fd, STDOUT_FILENO);
    int stderr_fd = dup2(error_fd, STDERR_FILENO);

    if (stdin_fd < 0 || stdout_fd < 0 || stderr_fd < 0 || stdout_fd_backup < 0) return -1;

    close(input_fd);
    close(output_fd);
    close(error_fd);

    if (!fork()) {
        char buffer;
        char line[1024];
        int i = 0;
        while ((read(STDIN_FILENO, &buffer, 1)) > 0) {
            line[i++] = buffer;
            if (buffer == '\n') {
                write(STDOUT_FILENO, line, i);
                write(STDERR_FILENO, line, i);
                i = 0;
            }
        }
        _exit(0);
    } else {
        int status;
        wait(&status);
        dup2(stdout_fd_backup, STDOUT_FILENO);
        close(stdout_fd_backup);

        char str[1024];
        WIFEXITED(status) ? sprintf(str, "child process returned %d\n", WEXITSTATUS(status))
                          : sprintf(str, "child process didnt terminate\n");
        write(STDOUT_FILENO, str, strlen(str) + 1);
    }
    return 0;
}
