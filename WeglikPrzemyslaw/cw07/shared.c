#include "shared.h"

int get_semaphore()
{
    key_t sem_key = ftok(getenv("HOME"), 1);
    int semaphore_id = semget(sem_key, 0, 0);
    if (semaphore_id < 0)
    {
        printf("Can't get semaphore %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return semaphore_id;
}

int get_oven_shared_memory()
{
    key_t shm_key = ftok(getenv("HOME"), 2);
    int shared_memory_id = shmget(shm_key, 0, 0);
    if (shared_memory_id < 0)
    {
        printf("Can't access shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return shared_memory_id;
}

int get_table_shared_memory()
{
    key_t shm_key = ftok(getenv("HOME"), 3);
    int shared_memory_id = shmget(shm_key, 0, 0);
    if (shared_memory_id < 0)
    {
        printf("Can't access shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return shared_memory_id;
}