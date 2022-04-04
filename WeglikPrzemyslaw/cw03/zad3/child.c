#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>


const unsigned MAX_LENGTH = 256;

DIR * open_dirstream(char * dirpath)
{
    // open directory stream
    DIR * dirstream = opendir(dirpath);

    if(dirstream == NULL)
    {
        printf("Couldn't open directory stream");
        exit(1);
    }
    return dirstream;
}

// returns 0 if found pattern, -1 if not found
int check_file(char * filename, char * pattern)
{
    FILE * file;
    file = fopen(filename, "r");
    if(file == NULL)
    {   
        printf("Couldn't open input file: %s", filename);
        exit(1);
    }

    char line[MAX_LENGTH];

    while (fgets(line, MAX_LENGTH, file))
    {
        if(strstr(line, pattern) != NULL)
        {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return -1;
}

void process_directory(char * dirpath, int depth, int max_depth, char * pattern)
{
    if(depth > max_depth)
    {
        return;
    }   
    DIR * dirstream = open_dirstream(dirpath);

    // iterate over catalogues
    struct dirent *dir = NULL;


    while((dir = readdir(dirstream)) != NULL)
    {
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        } 

        char relative_path[MAX_LENGTH + 1];
        strcpy(relative_path, dirpath);
        strcat(relative_path, "/");
        strcat(relative_path, dir->d_name);

        struct stat stat_info;

        if (lstat(relative_path, &stat_info) == -1) 
        {
            printf("Error when getting statistics!");
            exit(1);
        }

        bool dir_flag = false;
        switch (stat_info.st_mode & S_IFMT)
        {
            case S_IFDIR: 
                dir_flag = true;
                break;
            case S_IFREG: 
                if(check_file(relative_path, pattern) == 0)
                {
                    printf("MATCH! %s\n", relative_path);
                }
                break;
        }

        // call recursively
        if(dir_flag == true)
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
                strcpy(child_agrv[1], relative_path);
                // current depth
                child_agrv[2] = calloc(sizeof(char), 5);
                snprintf(temp_buff, 100, "%d", depth + 1);
                strcpy(child_agrv[2], temp_buff);
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
                    free(child_agrv[0]);
                    exit(1);
                }
                else
                {
                    free(child_agrv[0]);
                    free(child_agrv[1]);
                    free(child_agrv[2]);
                    free(child_agrv[3]);
                    free(child_agrv[4]);
                    free(child_agrv);
                    exit(0);
                }
            }
            else
            {
                waitpid(child_pid, NULL, 0);
            }

            
        }
    }
    
    closedir(dirstream);
}



/*
    Child process should get:
        * realtive path
        * current depth
        * max depth
        * pattern
*/
int main(int argc, char ** argv)
{
    // handling args
    int depth, max_depth;
    char * relative_path = calloc(sizeof(char), 100);
    char * pattern = calloc(sizeof(char), 100);
    if(argc == 5)
    {
        strcpy(relative_path, argv[1]);
        depth = atoi(argv[2]);
        max_depth = atoi(argv[3]);
        strcpy(pattern, argv[4]);
    }
    else
    {
        printf("Incorrect arguments!");
        return 1;
    }

    process_directory(relative_path, depth, max_depth, pattern);
    free(relative_path);
    free(pattern);

    return 0;
}