#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

struct temp_file_result
{
    int filedes;
    char name[21];
};

char** create_array(int array_size) {
    char **pointer = calloc(array_size, sizeof(char*));    
    return pointer;
}

struct temp_file_result count_words_and_save_to_tmpfile(char* files)
{
    // CREATING TMPFILE

    // buffer to hold the temporary file name
    char nameBuff[19];

    int filedes = -1;

    // memset the buffers to 0
    memset(nameBuff,0,sizeof(nameBuff));

    strncpy(nameBuff,"/tmp/result-XXXXXX",18);

    // Create the temporary file, this function will replace the 'X's
    filedes = mkstemp(nameBuff);

    // COUNTING
    char* command = calloc(strlen(files) + 8 + 4 + 19, sizeof(char));

    strcpy(command, "wc -lwc ");
    strcat(command, files);
    strcat(command, " > ");
    strcat(command, nameBuff);

    system(command);

    struct temp_file_result result;
    result.filedes = filedes;
    strcpy(result.name, nameBuff);

    free(command);
    return result;
}

int create_and_add_block(struct temp_file_result result, char** array, int array_size)
{
    int file_size = lseek(result.filedes, 0, SEEK_END);

    // reset pointer to start of the file
    lseek(result.filedes,0,SEEK_SET);

    char* buffer = calloc(file_size + 1, sizeof(char));
    read(result.filedes, buffer, file_size);

    int i = 0;
    
    while(i < array_size)
    {
        if(array[i] == NULL)
        {
            array[i] = buffer;
            break;
        }
        i += 1;
    }
    unlink(result.name);
    return  i;
}

void delete_block(char** array, int index)
{
    free(array[index]);
    array[index] = NULL;
}