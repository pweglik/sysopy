#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <dirent.h>

struct count_files
{
    int regular;
    int directory;
    int block_dev;
    int char_dev;
    int fifo;
    int symlink;
    int socket;
};

void process_arguments(int argc, char *argv[], char ** dirpath)
{
    if(argc == 2)
    {
        printf("Correct arguments! Copying file's content...\n");
        *dirpath = calloc(sizeof(char), PATH_MAX);
        strcpy(*dirpath, argv[1]);

    }
    else
    {
        *dirpath = calloc(sizeof(char), PATH_MAX);
        printf("You haven't passed right arguments!\n");
        printf("Directory's path: ");
        scanf("%s", *dirpath);
    }
}

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

void process_directory(char * dirpath, struct count_files * result)
{
    DIR * dirstream = open_dirstream(dirpath);

    // iterate over catalogues
    struct dirent *dir = NULL;


    while((dir = readdir(dirstream)) != NULL)
    {
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }

        char absolute_path[PATH_MAX + 1];
        char relative_path[PATH_MAX + 1];

        strcpy(relative_path, dirpath);
        strcat(relative_path, "/");
        strcat(relative_path, dir->d_name);

        realpath(relative_path, absolute_path);

        printf("Absolute path: %s\n", absolute_path);
        printf("Name: %s\n", dir->d_name);

        printf("File type: ");
        bool dir_flag = false;
        switch (dir->d_type)
        {
            case DT_BLK: 
                printf("This is a block device.");
                result->block_dev++;
                break;
            case DT_CHR: 
                printf("This is a character device.");
                result->char_dev++;
                break;
            case DT_DIR: 
                printf("This is a directory.");
                result->directory++;
                dir_flag = true;
                break;
            case DT_FIFO: 
                printf("This is a named pipe (FIFO).");
                result->fifo++;
                break;
            case DT_LNK: 
                printf("This is a symbolic link.");
                result->symlink++;
                break;
            case DT_REG: 
                printf("This is a regular file.");
                result->regular++;
                break;
            case DT_SOCK: 
                printf("This is a UNIX domain socket.");
                result->socket++;
                break;
            case DT_UNKNOWN: 
                printf("The file type could not be determined.");
                break;
        }
        printf("\n");

        struct stat stat_info;

        if (lstat(absolute_path, &stat_info) == -1) 
        {
            printf("Error when getting statistics!");
            exit(1);
        }

        printf("Size: %ld\n", stat_info.st_size);

        char datetime_buffer[100];

        strftime(datetime_buffer, sizeof datetime_buffer, "%D %T", gmtime(&stat_info.st_atim.tv_sec));
        printf("Last access: %s\n", datetime_buffer);

        strftime(datetime_buffer, sizeof datetime_buffer, "%D %T", gmtime(&stat_info.st_mtim.tv_sec));
        printf("Last modification: %s\n", datetime_buffer);

        printf("\n");
        // call recursively
        if(dir_flag == true)
        {
            process_directory(absolute_path, result);
        }
    }

    closedir(dirstream);
}

void print_counting_results(struct count_files result)
{
    printf("# of regular files: %d\n", result.regular);
    printf("# of directories: %d\n", result.directory);
    printf("# of block devices files: %d\n", result.block_dev);
    printf("# of character devices: %d\n", result.char_dev);
    printf("# of named pipes: %d\n", result.fifo);
    printf("# of symbolic links: %d\n", result.symlink);
    printf("# of sockets: %d\n", result.socket);
}



int main(int argc, char *argv[])
{
    char * dirpath;
    
    process_arguments(argc, argv, &dirpath);

    struct count_files result = {0,0,0,0,0,0,0};


    process_directory(dirpath, &result);

    print_counting_results(result);

    free(dirpath);

    return 0;

} 