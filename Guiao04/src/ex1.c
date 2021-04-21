#include <fcntl.h>
#include <unistd.h>

int main() {
    int input_fd = open("/etc/passwd", O_RDONLY);
    int output_fd = open("saida.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int error_fd = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    int stdin_fd = dup2(input_fd, STDIN_FILENO);
    int stdout_fd = dup2(output_fd, STDOUT_FILENO);
    int stderr_fd = dup2(error_fd, STDERR_FILENO);

    if (stdin_fd < 0 || stdout_fd < 0 || stderr_fd < 0) return -1;

    close(input_fd);
    close(output_fd);
    close(error_fd);

    int i = 0;
    char buffer;
    char line[1024];
    while ((read(STDIN_FILENO, &buffer, 1)) > 0) {
        line[i++] = buffer;
        if (buffer == '\n') {
            write(STDOUT_FILENO, line, i);
            write(STDERR_FILENO, line, i);
            i = 0;
        }
    }
    return 0;
}
