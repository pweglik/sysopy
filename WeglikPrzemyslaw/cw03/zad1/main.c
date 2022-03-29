#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char ** argv)
{
    int count_of_child_processes;
    if(argc == 2)
    {
        printf("Correct arguments! Running...\n\n");
        count_of_child_processes = atoi(argv[1]);
    }
    else
    {
        printf("Incorrect arguments! Pass number of child processes as first and only argument.");
        return 1;
    }


    for(int i = 0; i < count_of_child_processes; i++)
    {
        pid_t child_pid;
        child_pid = fork();

        if(child_pid == 0) {
            char* const child_agrv[]= {NULL};
            if(execv("./bin/child", child_agrv) == -1)
            {
                printf("Error while calling exec in child process!");
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            printf("Waiting for child with pid %d terminate!\n", child_pid);
            waitpid(child_pid, NULL, 0);
            printf("Done!\n");
        }
        
    }
    

    return 0;
}