#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

void P(int semId)
{
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;
    if (semop(semId, &op, 1) == -1)
    {
        perror("semop P");
        exit(EXIT_FAILURE);
    }
}

void V(int semId)
{
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    if (semop(semId, &op, 1) == -1)
    {
        perror("semop V");
        exit(EXIT_FAILURE);
    }
}
