#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_TERMS 16
#define MAX_PROGRAMS 32
#define MAX_ARG_COUNT 16
#define MAX_CMD_LEN 10000

void fill_program_with_arguments(char *args[], char *cmd)
{
    char *args_tmp = cmd;
    char *arg = strtok_r(cmd, " ", &args_tmp);
    int arg_counter = 0;
    while (arg != NULL)
    {
        args[arg_counter++] = arg;
        arg = strtok_r(NULL, " ", &args_tmp);
    }
}

void execute_command(char *line)
{
    char *programs[MAX_PROGRAMS][MAX_ARG_COUNT];

    for (int i = 0; i < MAX_PROGRAMS; ++i)
    {
        for (int j = 0; j < MAX_ARG_COUNT; ++j)
        {
            programs[i][j] = NULL;
        }
    }
    char *cmd_tmp = line;
    char *cmd = strtok_r(line, "|", &cmd_tmp);
    int prog_count = 0;
    while (cmd != NULL)
    {
        fill_program_with_arguments(programs[prog_count++], cmd);
        cmd = strtok_r(NULL, "|", &cmd_tmp);
    }
    int pipes[MAX_PROGRAMS][2];
    for (int i = 0; i < prog_count - 1; i++)
    {

        if (pipe(pipes[i]) < 0)
        {
            fprintf(stderr, "cant make pipe\n");
            exit(1);
        }
    }

    for (int i = 0; i < prog_count; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "cant fork\n");
            exit(1);
        }
        else if (pid == 0)
        {
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i + 1 < prog_count)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < prog_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(programs[i][0], programs[i]);
            exit(0);
        }
    }
    for (int j = 0; j < prog_count - 1; ++j)
    {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
    for (int i = 0; i < prog_count; ++i)
    {
        wait(0);
    }
}

long get_file_size(FILE *f)
{
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return file_size;
}

char ** get_term(char * line) 
{
    char ** term = malloc(2 * sizeof(char*));

    char *line_tmp = line;
    char *token = strtok_r(line, "=", &line_tmp);
    if (line != NULL)
    {
        term[0] = token;
        term[0][strlen(term[0])-1] = '\0';
        token = strtok_r(NULL, "\n", &line_tmp);
        term[1] = token;
    }

    return term;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong arguments, usage: [file]\n");
        exit(1);
    }
    char *file_path = argv[1];
    FILE *f = fopen(file_path, "r");
    if (f == NULL)
    {
        fprintf(stderr, "can't find or open file %s\n", file_path);
        exit(1);
    }

    long file_size = get_file_size(f);
    char *buffer = malloc(file_size + 1);
    if (fread(buffer, 1, file_size, f) != file_size)
    {
        fprintf(stderr, "cant read from file %s\n", file_path);
        exit(1);
    }
    fclose(f);

    char ** terms[MAX_TERMS]; // first field is name of term, second is content

    int mode = 0; // 0 - reading terms, 1 - reading and executing commands build from terms
    int term_counter = 0;

    char *lines_tmp = buffer;
    char *line = strtok_r(buffer, "\n", &lines_tmp);
    while (line != NULL)
    {
        if(strcmp(line, "EXECUTE") == 0)
        {
            mode = 1;
        }
        else if (mode == 0)
        {
            char * line_copy = malloc( strlen(line) + 1 ); 
            strcpy(line_copy, line);

            terms[term_counter] = get_term(line_copy);
            // printf("%s...%s \n", terms[term_counter][0], terms[term_counter][1]);
            term_counter++;
        }
        else if (mode == 1)
        {
            char command[MAX_CMD_LEN];
            strcpy(command, "");

            char *line_tmp = line;
            char *token = strtok_r(line, "|", &line_tmp);
            while (token != NULL)
            {
                for(int i = 0; i < term_counter; i++)
                {
                    if(strcmp(token, terms[i][0]) == 0)
                    {
                        strcat(command, terms[i][1]);
                        strcat(command, " |");
                        break;
                    }
                }
                token = strtok_r(NULL, "\n", &line_tmp);
            }
            command[strlen(command) - 1] = '\0';
            printf("COMMAND: %s\n", command);
            execute_command(command);
        }

        line = strtok_r(NULL, "\n", &lines_tmp);
    }
    free(buffer);
    return 0;
}