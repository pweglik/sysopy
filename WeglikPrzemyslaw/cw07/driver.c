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
int table_shared_memory_id;

typedef struct sembuf sembuf;

void deliver_pizza()
{
    // printf("driver: %d %d %d %d %d %d\n", 
    // semctl(semaphore_id, 0, GETVAL, NULL), 
    // semctl(semaphore_id, 1, GETVAL, NULL),
    // semctl(semaphore_id, 2, GETVAL, NULL),
    // semctl(semaphore_id, 3, GETVAL, NULL),
    // semctl(semaphore_id, 4, GETVAL, NULL),
    // semctl(semaphore_id, 5, GETVAL, NULL));
    // taking pizza from the table
    sembuf *lock_table = calloc(2, sizeof(sembuf));

    lock_table[1].sem_num = LOCK_TABLE;
    lock_table[1].sem_op = -1;
    lock_table[1].sem_flg = 0;

    semop(semaphore_id, lock_table, 2);

    table *tbl = shmat(table_shared_memory_id, NULL, 0);

    int index = (semctl(semaphore_id, ALL_TABLE, GETVAL, NULL) - 1) % MAX_TABLE;
    int value = tbl->values[index];
    tbl->values[index] = -1;

    shmdt(tbl);

    sembuf *deliver = calloc(2, sizeof(sembuf));

    deliver[0].sem_num = COUNT_TABLE;
    deliver[0].sem_op = -1;
    deliver[0].sem_flg = 0;

    deliver[1].sem_num = LOCK_TABLE;
    deliver[1].sem_op = 1;
    deliver[1].sem_flg = 0;

    semop(semaphore_id, deliver, 2);

    int count_table = semctl(semaphore_id, COUNT_TABLE, GETVAL, NULL);
    printf("(%d %ld) Pobieram pizze: %d Liczba pizz na stole: %d.\n",getpid(), time(NULL), value, count_table);

    // delivering
    usleep(rand_wait);

    printf("(%d %ld) Dostarczam pizze: %d.\n",getpid(), time(NULL), value);

    // coming back
    usleep(rand_wait);
}

int main()
{
    srand(time(NULL));

    semaphore_id = get_semaphore();
    table_shared_memory_id = get_table_shared_memory();

    while (1)
    {
        if (semctl(semaphore_id, COUNT_TABLE, GETVAL, NULL) > 0)
        {
            deliver_pizza();
        }
    }

    return 0;
}