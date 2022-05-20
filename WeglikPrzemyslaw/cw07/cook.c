#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "shared.h"

int semaphore_id;
int oven_shared_memory_id;
int table_shared_memory_id;

typedef struct sembuf sembuf;

void make_pizza()
{
    // prepare pizza
    int value = rand_pizza_num;

    printf("(%d %ld) Przygotowuje pizze: %d.\n", getpid(), time(NULL), value);
    usleep(rand_prep_time);

    // put pizza into oven
    sembuf *lock = calloc(1, sizeof(sembuf));

    lock[0].sem_num = LOCK_OVEN;
    lock[0].sem_op = -1;
    lock[0].sem_flg = 0;

    semop(semaphore_id, lock, 1);

    oven *ov = shmat(oven_shared_memory_id, NULL, 0);
    int index = (semctl(semaphore_id, ALL_OVEN, GETVAL, NULL)) % MAX_OVEN;
    ov->values[index] = value;

    shmdt(ov);

    sembuf *put_oven = calloc(3, sizeof(sembuf));

    put_oven[0].sem_num = COUNT_OVEN;
    put_oven[0].sem_op = 1;
    put_oven[0].sem_flg = 0;

    put_oven[1].sem_num = ALL_OVEN;
    put_oven[1].sem_op = 1;
    put_oven[1].sem_flg = 0;

    put_oven[2].sem_num = LOCK_OVEN;
    put_oven[2].sem_op = 1;
    put_oven[2].sem_flg = 0;

    semop(semaphore_id, put_oven, 3);

    int count_oven = semctl(semaphore_id, COUNT_OVEN, GETVAL, NULL);

    printf("(%d %ld) Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), time(NULL), value, count_oven);
    
    // waiting for pizza to bake
    usleep(rand_wait);
    // taking pizza out and putting it on a table

    // lock oven
    semop(semaphore_id, lock, 1);


    ov = shmat(oven_shared_memory_id, NULL, 0);
    int taken_out_value = ov->values[index];
    ov->values[index] = -1;

    shmdt(ov);
    // unlock oven
    sembuf *unlock_oven = calloc(2, sizeof(sembuf));
    
    unlock_oven[0].sem_num = COUNT_OVEN;
    unlock_oven[0].sem_op = -1;
    unlock_oven[0].sem_flg = 0;

    unlock_oven[1].sem_num = LOCK_OVEN;
    unlock_oven[1].sem_op = 1;
    unlock_oven[1].sem_flg = 0;

    semop(semaphore_id, unlock_oven, 2);
    // wait for free place on table
    while (1)
    {
        if (semctl(semaphore_id, COUNT_TABLE, GETVAL, NULL) < MAX_TABLE)
        {
            break;
        }
    }
    
    // put pizza on the table
    sembuf *lock_table = calloc(1, sizeof(sembuf));

    lock_table[0].sem_num = LOCK_TABLE;
    lock_table[0].sem_op = -1;
    lock_table[0].sem_flg = 0;

    semop(semaphore_id, lock_table, 1);

    table *tbl = shmat(table_shared_memory_id, NULL, 0);
    index = (semctl(semaphore_id, ALL_TABLE, GETVAL, NULL)) % MAX_TABLE;
    tbl->values[index] = taken_out_value;

    shmdt(tbl);

    sembuf *put_table = calloc(3, sizeof(sembuf));
    put_table[0].sem_num = COUNT_TABLE;
    put_table[0].sem_op = 1;
    put_table[0].sem_flg = 0;

    put_table[1].sem_num = ALL_TABLE;
    put_table[1].sem_op = 1;
    put_table[1].sem_flg = 0;

    put_table[2].sem_num = LOCK_TABLE;
    put_table[2].sem_op = 1;
    put_table[2].sem_flg = 0;

    semop(semaphore_id, put_table, 3);

    // printf("cook: %d %d %d %d %d %d\n", 
    // semctl(semaphore_id, 0, GETVAL, NULL), 
    // semctl(semaphore_id, 1, GETVAL, NULL),
    // semctl(semaphore_id, 2, GETVAL, NULL),
    // semctl(semaphore_id, 3, GETVAL, NULL),
    // semctl(semaphore_id, 4, GETVAL, NULL),
    // semctl(semaphore_id, 5, GETVAL, NULL));
    count_oven = semctl(semaphore_id, COUNT_OVEN, GETVAL, NULL);
    int count_table = semctl(semaphore_id, COUNT_TABLE, GETVAL, NULL);

    printf("(%d %ld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), time(NULL), taken_out_value, count_oven, count_table);

}

int main()
{
    srand(time(NULL));

    semaphore_id = get_semaphore();
    oven_shared_memory_id = get_oven_shared_memory();
    table_shared_memory_id = get_table_shared_memory();

    while (1)
    {
        if (semctl(semaphore_id, COUNT_OVEN, GETVAL, NULL) < MAX_OVEN)
        {
            make_pizza();
        }
    }
}