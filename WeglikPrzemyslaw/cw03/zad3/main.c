#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void init_call(char * dirpath, int max_depth, char * pattern)
{
    pid_t child_pid;
    child_pid = fork();

    if(child_pid == 0) {
        
        char ** child_agrv = calloc(sizeof(char *), 5);
        char temp_buff[100];
        //name of program
        child_agrv[0] = calloc(sizeof(char), 6);
        strcpy(child_agrv[0], "child");
        //relative path
        child_agrv[1] = calloc(sizeof(char), 100);
        strcpy(child_agrv[1], dirpath);
        // current depth
        child_agrv[2] = calloc(sizeof(char), 5);
        strcpy(child_agrv[2], "0");
        // max depth
        child_agrv[3] = calloc(sizeof(char), 5);
        snprintf(temp_buff, 100, "%d", max_depth);
        strcpy(child_agrv[3], temp_buff);
        // pattern
        child_agrv[4] = calloc(sizeof(char), 50);
        strcpy(child_agrv[4], pattern);
        // starting point

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

/*
Arguments:
    * root of seach tree
    * searched pattern
    * max depth
*/
int main(int argc, char ** argv)
{
    char * root_dir_name;
    int max_depth;
    char * pattern;
    if(argc == 4)
    {
        printf("Correct arguments! Running...\n\n");
        root_dir_name = calloc(sizeof(char), strlen(argv[1]));
        strcpy(root_dir_name, argv[1]);
        pattern = calloc(sizeof(char), strlen(argv[2]));
        strcpy(pattern, argv[2]);
        max_depth = atoi(argv[3]);
    }
    else
    {
        printf("Incorrect arguments!");
        return 1;
    }

    init_call(root_dir_name, max_depth, pattern);

    free(root_dir_name);
    free(pattern);

    return 0;
}