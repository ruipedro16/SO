#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned int seconds = 0;
unsigned int ctrl_c_counter = 0;

void sigint_handler(int signum) {
    ctrl_c_counter++;
    printf("CTRL+C received [signum %d]. %d seconds have passed\n", signum, seconds);
}

void sigquit_handler(int signum) {
    printf("CTRL+\\ received [signum %d]. CTRL+C was called %d times\n", signum, ctrl_c_counter);
    exit(0);
}

void sigalrm_handler(int signum) {
    seconds++;
    alarm(1);
}

int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("SIGINT failed");
    }

    if (signal(SIGQUIT, sigquit_handler) == SIG_ERR) {
        perror("SIGQUIT failed");
    }

    if (signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
        perror("SIGALRM failed");
    }

    alarm(1);

    while (1) {
        pause();
    }

    return 0;
}
