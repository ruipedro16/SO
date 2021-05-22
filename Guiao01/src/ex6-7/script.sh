#!/bin/sh

gcc person.c person.h main.c -o pessoas

for ((i = 1; i <= $1; i++))
do
    IDADE=$(((RANDOM % 100) + 1))
    echo "./pessoas -i Pessoa${i} $IDADE"

    ./pessoas -i Pessoa${i} $IDADE
done

