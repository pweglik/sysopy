#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <sys/times.h>


int MAX_FILENAME = 100;

void process_arguments(int argc, char *argv[], char ** character_to_count, char ** input_filename)
{
    if(argc == 3)
    {
        printf("Correct arguments! Copying file's content...\n");
        *character_to_count = calloc(sizeof(char), strlen(argv[1]));
        strcpy(*character_to_count, argv[1]);

        *input_filename = calloc(sizeof(char), strlen(argv[2]));
        strcpy(*input_filename, argv[2]);
    }
    else
    {
        *character_to_count = calloc(sizeof(char), MAX_FILENAME);
        *input_filename = calloc(sizeof(char), MAX_FILENAME);
        printf("You haven't passed right arguments!\n");
        printf("Character to count: ");
        scanf("%s", *character_to_count);
        printf("Input file: ");
        scanf("%s", *input_filename);
    }
}

void load_file(int * input_file, char ** input_filename)
{
    *input_file = open(*input_filename, O_RDONLY);
    if(*input_file == -1)
    {   
        printf("Couldn't open input file: %s", *input_filename);
        exit(1);
    }
}

struct count_results
{
    int character_count;
    int line_count;
};

struct count_results count_chars_and_lines(int * input_file, char * character_to_count)
{
    struct count_results result;
    result.character_count = 0;
    result.line_count = 0;

    char character[1];

    // flag to mark if we have already encountered a character
    bool char_in_line = false;

    while(read(*input_file, character, 1) == 1) 
    {
        if(strcmp(character, "\n") == 0)
        {
            if(char_in_line == true)
            {
                result.line_count++;
            }
            // cleaning line and len
            char_in_line = false;
        }
        else
        {
            if(strcmp(character, character_to_count) == 0)
            {
                result.character_count++;
                char_in_line = true;
            }
        }
    }

    return result;
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
    char * character_to_count;
    char * input_filename;
    
    process_arguments(argc, argv, &character_to_count, &input_filename);

    // start timing
    start_clock();

    int input_file;

    load_file(&input_file, &input_filename);

    struct count_results result = count_chars_and_lines(&input_file, character_to_count);
    printf("Character count: %d, Line count: %d \n", result.character_count, result.line_count);

    end_clock();

    close(input_file);

    free(input_filename);
    free(character_to_count);

    return 0;

} 