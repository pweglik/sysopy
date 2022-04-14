#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>


void create_subprocesses(int count_of_child_processes, int rectangles_per_process, int leftover, float width_of_rectangle)
{
    for(int id = 0; id < count_of_child_processes; id++)
    {
        pid_t child_pid;
        child_pid = fork();

        if(child_pid == 0) {
            char ** child_agrv = calloc(sizeof(char *), 5);
            char temp_buff[100];
            //name of program
            child_agrv[0] = calloc(sizeof(char), 6);
            strcpy(child_agrv[0], "child");
            //id
            child_agrv[1] = calloc(sizeof(char), 10);
            snprintf(temp_buff, 100, "%d", id);
            strcpy(child_agrv[1], temp_buff);
            // width of rectangle
            child_agrv[2] = calloc(sizeof(char), 20);
            snprintf(temp_buff, 100, "%f", width_of_rectangle);
            strcpy(child_agrv[2], temp_buff);
            // number of rectangles handles by a process
            // leftover rectangles are added up to 1st process
            child_agrv[3] = calloc(sizeof(char), 10);
            if(id == 0) snprintf(temp_buff, 100, "%d", rectangles_per_process + leftover);
            else snprintf(temp_buff, 100, "%d", rectangles_per_process);
            strcpy(child_agrv[3], temp_buff);
            // starting point
            child_agrv[4] = calloc(sizeof(char), 20);
            if(id == 0) snprintf(temp_buff, 100, "%f", 0.0f);
            else snprintf(temp_buff, 100, "%f", (rectangles_per_process * id + leftover) * width_of_rectangle);
            strcpy(child_agrv[4], temp_buff);

            if(execl("./bin/child", child_agrv[0], child_agrv[1], child_agrv[2], child_agrv[3], child_agrv[4], NULL) == -1)
            {
                printf("Error while calling exec in child process!\n");
                exit(1);
            }
            else
            {
                exit(0);
            }
        }
        else
        {
            waitpid(child_pid, NULL, 0);
        }
    }
}

void sum_results(int count_of_child_processes)
{
    float sum = 0;
    for(int id = 0; id < count_of_child_processes; id++)
    {
        char filename[100];
        char temp_buff[33];
        snprintf(temp_buff, 33, "%d", id);
        strcpy(filename, "data/w");
        strcat(filename, temp_buff);
        strcat(filename, ".txt");
        FILE * result_file = fopen(filename, "r");
        
        float partial_sum;
        if (fscanf(result_file, "%f", &partial_sum) == 1)
        {
            sum += partial_sum;
        }
        fclose(result_file);
    }

    printf("Value of integral is: %f\n", sum);
}

// functions used for timing
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

void start_clock()
{
    st_time = times(&st_cpu);
}

void end_clock()
{
    en_time = times(&en_cpu);
    double real_time = (double)(en_time - st_time) / (double) sysconf(_SC_CLK_TCK);
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / (double) sysconf(_SC_CLK_TCK);
    double sys_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / (double) sysconf(_SC_CLK_TCK);
    printf("Real Time: %.3f, User Time %.3f, System Time %.3f\n",
            real_time, user_time, sys_time);
            
}


/*
Arguments:
    * width of rectangles 
    * number of child process
*/

int main(int argc, char ** argv)
{
    float width_of_rectangle;
    int count_of_child_processes;
    if(argc == 3)
    {
        printf("Correct arguments! Running...\n\n");
        width_of_rectangle = atof(argv[1]);
        count_of_child_processes = atoi(argv[2]);
    }
    else
    {
        printf("Incorrect arguments! Pass number of child processes as first and only argument.");
        return 1;
    }

    start_clock();

    int count_of_rectangles = 1 / width_of_rectangle;
    int rectangles_per_process = count_of_rectangles / count_of_child_processes;
    int leftover = count_of_rectangles % count_of_child_processes;

    create_subprocesses(count_of_child_processes, rectangles_per_process, leftover, width_of_rectangle);

    sum_results(count_of_child_processes);

    end_clock();

    return 0;
}