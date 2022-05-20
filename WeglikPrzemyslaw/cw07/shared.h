#ifndef SHARED_H
#define SHARED_H

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
#define MAX_OVEN 5
#define MAX_TABLE 5
#define MIN_WAIT 4000
#define MAX_WAIT 5000
#define MIN_PREP_TIME 1000
#define MAX_PREP_TIME 2000
#define MIN_VAL_PIZZA 0
#define MAX_VAL_PIZZA 9

#define LOCK_OVEN 0
#define COUNT_OVEN 1
#define ALL_OVEN 2
#define LOCK_TABLE 3
#define COUNT_TABLE 4
#define ALL_TABLE 5

typedef struct
{
    int values[MAX_OVEN];
} oven;

typedef struct
{
    int values[MAX_TABLE];
} table;


#define rand_pizza_num (rand() % (MAX_VAL_PIZZA - MIN_VAL_PIZZA + 1) + MIN_VAL_PIZZA)

#define rand_wait ((rand() % (MAX_WAIT - MIN_WAIT + 1) + MIN_WAIT) * 1000)

#define rand_prep_time ((rand() % (MAX_PREP_TIME - MIN_PREP_TIME + 1) + MIN_PREP_TIME) * 1000)

int get_semaphore();
int get_oven_shared_memory();
int get_table_shared_memory();

#endif //SHARED_H