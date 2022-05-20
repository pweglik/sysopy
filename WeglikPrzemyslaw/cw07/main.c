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
#include <signal.h>

#include "shared.h"

#define N 2
#define M 5

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

pid_t pids[N + M];

int semaphore_id;
int oven_shared_memory_id;
int table_shared_memory_id;

void set_semaphore()
{
    key_t sem_key = ftok(getenv("HOME"), 1);
    semaphore_id = semget(sem_key, 6, IPC_CREAT | 0666); 
    // 4 semaphores: 0 - lock oven, 1 - pizzas in oven, 2 - num of all pizzas in oven, 
    // 3 - lock table, 4 - pizzas on table, 5- num of all pizzas on table
    if (semaphore_id < 0)
    {
        printf("Cannot create semaphores set %d\n", errno);
        exit(EXIT_FAILURE);
    }
    union semun arg;
    arg.val = 0;

    for (int i = 0; i < 6; i++)
    {   
        if(i == 0 || i == 3) 
        {
            arg.val = 1;
        }
        else
        {
            arg.val = 0;
        }
        semctl(semaphore_id, i, SETVAL, arg);
    }
}

void create_shared_memory()
{
    key_t shm_key = ftok(getenv("HOME"), 2);
    oven_shared_memory_id = shmget(shm_key, sizeof(oven), IPC_CREAT | 0666);
    if (oven_shared_memory_id < 0)
    {
        printf("Cannot create shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
    shm_key = ftok(getenv("HOME"), 3);
    table_shared_memory_id = shmget(shm_key, sizeof(table), IPC_CREAT | 0666);
    if (oven_shared_memory_id < 0)
    {
        printf("Cannot create shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void clear()
{
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(oven_shared_memory_id, IPC_RMID, NULL);
    shmctl(table_shared_memory_id, IPC_RMID, NULL);
    system("make clean");
}

void handle_SIGINT(int signum)
{
    for (int i = 0; i < N + M; i++)
    {
        kill(pids[i], SIGINT);
    }
    clear();
    exit(0);
}

void run_workers()
{
    for (int i = 0; i < N; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./cook", "cook", NULL);
        }
        pids[i] = child_pid;
    }

    for (int i = 0; i < M; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./driver", "driver", NULL);
        }
        pids[i + N] = child_pid;
    }

    for (int i = 0; i < N + M; i++)
    {
        wait(NULL);
    }
}

int main()
{
    signal(SIGINT, handle_SIGINT);
    set_semaphore();
    create_shared_memory();
    run_workers();
    clear();
    return 0;
}