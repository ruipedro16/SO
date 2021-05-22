#include "person.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int new_person(const char *name, int age) {
    int r;

    Person p;
    strcpy(p.name, name);
    p.age = age;

    int fd = open(FILENAME, O_CREAT | O_APPEND | O_WRONLY, 0600);

    if (fd < 0) {
        perror("Error accessing file");
        return -1;
    } else {
        ssize_t bytes_written;
        if ((bytes_written = write(fd, &p, sizeof(Person)) == sizeof(Person))) {
            r = lseek(fd, -sizeof(Person), SEEK_CUR);
            if (r < 0) {
                perror("Error using lseek");
                return -1;
            }
        } else {
            perror("Error adding new person");
            return -1;
        }
        if ((fd = close(fd)) == -1) {
            perror("Error closing file");
            return -1;
        }
    }

    return r;
}

int set_age(const char *name, int age) {
    Person p;
    int fd;

    if ((fd = open(FILENAME, O_CREAT | O_RDWR, 0644)) < 0) {
        perror("Error accessing file");
        return -1;
    } else {
        while (read(fd, &p, sizeof(Person)) > 0) {
            if (!(strcmp(p.name, name))) {
                int pos = lseek(fd, -sizeof(Person), SEEK_CUR);
                if (pos >= 0) {
                    p.age = age;
                    int bytes_written = write(fd, &p, sizeof(Person));
                    if (bytes_written != sizeof(Person)) {
                        perror("Error writting");
                        return -1;
                    }
                } else {
                    perror("Error in lseek");
                    return -1;
                }
            }
        }

        fd = close(fd);
        if (fd == -1) {
            perror("Error closing file");
            return -1;
        }
    }

    return 0;
}

int set_age_v2(long pos, int age) {
    int fd;
    if ((fd = open(FILENAME, O_CREAT | O_WRONLY, 0600)) < 0) {
        perror("Error accessing file");
        return -1;
    } else {
        int updated = 0;
        Person p;

        while ((read(fd, &p, sizeof(Person))) > 0 && !updated) {
            int seek_res = lseek(fd, 0, SEEK_CUR);
            if (seek_res < 0) {
                perror("Error lseek");
                close(fd);
                return -1;
            }
            if (pos == seek_res) {
                p.age = age;
                if (write(fd, &p, sizeof(Person)) < 0) {
                    perror("write");
                    close(fd);
                    return -1;
                }
                updated = 1;
            }
        }
        if ((fd = close(fd)) == -1) {
            perror("Error closing file");
            return -1;
        }
        return updated;
    }
}
