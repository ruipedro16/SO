#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define COLUMNS 1000
#define ROWS 10

int main() {
    int fd = open("matriz", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd < 0) return -1;

    // Generate random matrix & write to file
    srand(time(NULL));
    int matrix[ROWS][COLUMNS];
    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLUMNS; j++) {
            int value = rand() % 50;
            matrix[i][j] = value;
            write(fd, &value, sizeof(int));
        }
    }
    close(fd);

    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLUMNS; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    int num = rand() % 50;

    // 1 process per row
    for (size_t i = 0; i < ROWS; i++) {
        if (!fork()) {
            int m = 0;
            for (size_t j = 0; j < COLUMNS; j++) {
                if (matrix[i][j] == num) m++;
            }
            _exit(m);
        }
    }

    int ocurrences = 0;

    for (size_t i = 0; i < ROWS; i++) {
        int status;
        wait(&status);
        if (WEXITSTATUS(status)) ocurrences += WEXITSTATUS(status);
    }

    printf("%d ocurrences of %d were found.\n", ocurrences, num);

    return 0;
}
