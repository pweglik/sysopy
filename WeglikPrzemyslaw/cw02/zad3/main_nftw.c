#include <unistd.h>
#include <dirent.h>
#define __USE_XOPEN_EXTENDED 1
#include <ftw.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>


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

struct count_files result = {0,0,0,0,0,0,0};

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

int process_file(const char *fpath, const struct stat *stat_info, int typeflag, struct FTW *ftwbuf)
{
    char absolute_path[PATH_MAX + 1];

    realpath( fpath, absolute_path);

    printf("Absolute path: %s\n", absolute_path);
    printf("Name: %s\n", fpath);

    printf("File type: ");
    switch (stat_info->st_mode & S_IFMT)
    {
        case S_IFBLK: 
            printf("This is a block device.");
            result.block_dev++;
            break;
        case S_IFCHR: 
            printf("This is a character device.");
            result.char_dev++;
            break;
        case S_IFDIR: 
            printf("This is a directory.");
            result.directory++;
            break;
        case S_IFIFO: 
            printf("This is a named pipe (FIFO).");
            result.fifo++;
            break;
        case S_IFLNK: 
            printf("This is a symbolic link.");
            result.symlink++;
            break;
        case S_IFREG: 
            printf("This is a regular file.");
            result.regular++;
            break;
        case S_IFSOCK: 
            printf("This is a UNIX domain socket.");
            result.socket++;
            break;

    }
    printf("\n");

    printf("Size: %ld\n", stat_info->st_size);

    char datetime_buffer[100];

    strftime(datetime_buffer, sizeof datetime_buffer, "%D %T", gmtime(&stat_info->st_atim.tv_sec));
    printf("Last access: %s\n", datetime_buffer);

    strftime(datetime_buffer, sizeof datetime_buffer, "%D %T", gmtime(&stat_info->st_mtim.tv_sec));
    printf("Last modification: %s\n", datetime_buffer);

    printf("\n");
    

    return 0;
}

void print_counting_results()
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

    int flags = 0;
    // flags = flags | FTW_PHYS;

    if (nftw(dirpath, process_file, 20, flags)
            == -1) {
        printf("Error while walking file tree");
        exit(1);
    }

    print_counting_results();

    free(dirpath);

    return 0;

} 