#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define COLUMNS 1000
#define ROWS 10

int main() {
    // Generate random matrix
    srand(time(NULL));
    int matrix[ROWS][COLUMNS];
    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLUMNS; j++) {
            matrix[i][j] = rand() % 50;
        }
    }

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
            if (m > 0)
                _exit(m);
            else
                _exit(0);
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
