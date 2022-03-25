#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <sys/times.h>


int LINE_MAX_LENGTH = 10000;
int MAX_FILENAME = 100;

void process_arguments(int argc, char *argv[], char ** input_filename, char ** output_filename)
{
    if(argc == 3)
    {
        printf("Correct arguments! Copying file's content...\n");
        *input_filename = calloc(sizeof(char), strlen(argv[1]));
        strcpy(*input_filename, argv[1]);

        *output_filename = calloc(sizeof(char), strlen(argv[2]));
        strcpy(*output_filename, argv[2]);
    }
    else
    {
        *input_filename = calloc(sizeof(char), MAX_FILENAME);
        *output_filename = calloc(sizeof(char), MAX_FILENAME);
        printf("You haven't passed right arguments!\n");
        printf("Input file: ");
        scanf("%s", *input_filename);
        printf("Output file: ");
        scanf("%s", *output_filename);
    }
}

void load_files(int * input_file, int * output_file, char ** input_filename, char ** output_filename)
{
    *input_file = open(*input_filename, O_RDONLY);
    if(*input_file == -1)
    {   
        printf("Couldn't open input file: %s", *input_filename);
        exit(1);
    }

    *output_file = open(*output_filename, O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    if(*output_file == -1)
    {   
        printf("Couldn't open output file: %s", *output_filename);
        exit(1);
    }
}

bool is_line_empty(char * line)
{
    size_t i = 0;
    while(line[i] != '\0')
    {
        if(isspace(line[i]) == 0)
        {
            return false;
        }
        i++;
    }
    return true;
}

void filter_empty_lines(int * input_file, int * output_file)
{
    char * line = calloc(sizeof(char), LINE_MAX_LENGTH);
    char character[1];
    size_t len;

    strcpy(line, "");
    len = 0;

    while(read(*input_file, character, 1) == 1) {
        if(strcmp(character, "\n") == 0)
        {
            if(is_line_empty(line) == false)
            {
                strcat(line, "\n");
                len += 1;
                write(*output_file, line, len);
            }
            // cleaning line and len
            strcpy(line, "");
            len = 0;
        }
        else
        {
            strcat(line, character);
            len += 1;
        }
    }

    free(line);
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

int main(int argc, char *argv[])
{
    char * input_filename;
    char * output_filename;
    
    process_arguments(argc, argv, &input_filename, &output_filename);

    // start timing
    start_clock();

    int input_file;
    int output_file;

    load_files(&input_file, &output_file, &input_filename, &output_filename);

    filter_empty_lines(&input_file, &output_file);

    end_clock();

    close(input_file);
    close(output_file);

    free(input_filename);
    free(output_filename);

    return 0;

} 
