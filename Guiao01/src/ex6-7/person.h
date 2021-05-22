#ifndef __PERSON_H__
#define __PERSON_H__

#define FILENAME "file_pessoas.bin"

typedef struct {
    char name[20];
    int age;
} Person;

int new_person(const char *name, int age);
int set_age(const char *name, int age);
int set_age_v2(long pos, int age);

#endif
