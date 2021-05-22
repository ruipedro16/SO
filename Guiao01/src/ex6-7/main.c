#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "person.h"

#define MAXBUFSIZE 1024

int isNumber(const char *str) {
    regex_t regex;
    int value;
    value = regcomp(&regex, "[1-9][0-9]*", 0);
    if (value) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }
    value = regexec(&regex, str, 0, NULL, 0);
    if (!value) {
        return 1;
    } else if (value == REG_NOMATCH) {
        return 0;
    } else {
        fprintf(stderr, "Regex match failed\n");
        exit(1);
    }
}

int main(int argc, char const *argv[]) {
    char *buffer = malloc(MAXBUFSIZE);

    if (!(strcmp(argv[1], "-i"))) {
        int pos = new_person(argv[2], atoi(argv[3]));
        printf("register %d\n", pos);
    } else if (!(strcmp(argv[1], "-u"))) {
        int ret;
        if (isNumber(argv[2])) {
            if ((ret = set_age_v2(atol(argv[2]), atoi(argv[3])))) {
                perror("Error setting age (v2)");
            }
        } else {
            if ((ret = set_age(argv[2], atoi(argv[3])))) {
                perror("Error setting age (v1)");
            }
        }
    }

    free(buffer);
    return 0;
}
