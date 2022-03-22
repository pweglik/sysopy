#include <sys/times.h>

#include <dlfcn.h>
#include "lib/alloc_library.c"


int create_array_handler(char* token, char*** array)
{
    // free in case it is already declared
    if(*array != NULL)
    {
        free(*array);
    }

    token = strtok(NULL, " ");
    token[strlen(token) - 1] = '\0';
    int array_size = -1;
    if(token != NULL)
    {
        array_size = atoi(token); // returns 0 on error
    }

    if(strtok(NULL, " ") == NULL)
    {
        *array = create_array(array_size);
    }

    return array_size;
}

struct temp_file_result wc_files_handler(char *token)
{ 
    size_t filenames_size = 200;
    char *filenames = calloc(sizeof(char), filenames_size);

    token = strtok(NULL, " ");
    while(token != NULL)
	{
		strcat(filenames, token);
        strcat(filenames, " ");
		token = strtok(NULL, " ");
	}
    struct temp_file_result result;
    filenames[strlen(filenames) - 2] = '\0';
    result = count_words_and_save_to_tmpfile(filenames);
    return result;
}

int create_and_add_block_handler(struct temp_file_result result, char** array, int array_size)
{
    return create_and_add_block(result, array, array_size);
}

int delete_block_handler(char *token, char** array)
{
    token = strtok(NULL, " ");
    token[strlen(token) - 1] = '\0';
    int index = atoi(token);

    if(strtok(NULL, " ") == NULL)
    {
        delete_block(array, index);
    }
    else
    {
        index = -1;
    }
    return index;
}

// auxillary function for debugging purposes
void view_array(char** array, int array_size)
{
    int i = 0;
    
    while(i < array_size)
    {  
        printf("i: %d", i);
        if( array[i] != NULL)
            printf(" NONEMPTY\n");
        else
            printf(" EMPTY\n");
        i++;
    }
    printf("\n");
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

    printf("Real Time: %.3f, User Time %.3f, System Time %.3f\n",
            (double)(en_time - st_time) / (double) sysconf(_SC_CLK_TCK),
            (double)(en_cpu.tms_utime - st_cpu.tms_utime) / (double) sysconf(_SC_CLK_TCK),
            (double)(en_cpu.tms_stime - st_cpu.tms_stime)) / (double) sysconf(_SC_CLK_TCK);
}

int main() {
    /*
    create_array <size of an array> - creates an array
    wc_files <list of filenames> - counts lines, words and chars in files and saves result to tmp file
    create_and_add_block - reads from tmp file and saves to block
    delete_block <index> - deletes block on given index
    */

    //run this if you want to load library dynamically
    
    #ifdef DYNAMIC_LOADING
        void *handle = dlopen("../zad2/lib/liballoc_library.so", RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Cannot load liballoc_library.so\n");
            exit(4);
        }


        char** (*create_array)(int) = (char** (*) (int)) dlsym(handle, "create_array");

        struct temp_file_result (*count_words_and_save_to_tmpfile)(char*) = (struct temp_file_result (*) (char*)) dlsym(handle, "count_words_and_save_to_tmpfile");

        int (*create_and_add_block)(struct temp_file_result, char**, int) = (int (*)( struct temp_file_result, char**, int)) dlsym(handle, "create_and_add_block");

        void (*delete_block)(int) = (void (*) (int)) dlsym(handle, "delete_block");

        if (dlerror()) {
            fprintf(stderr, "Cannot load functions from liballoc_library.so\n");
            exit(1);
        }
    #endif
    // Buffer for handling input
    size_t linebuffer_size = 100;
    char *linebuffer = malloc(linebuffer_size * sizeof(char));

    // Variables for functioning of program
    int array_size = -1;
    char **array = NULL;
    struct temp_file_result result;

    while(getline(&linebuffer, &linebuffer_size, stdin) != -1)
    {
        start_clock();
        // Tokenized text
        char *token = strtok(linebuffer, " ");
        if(token == NULL)
        {
            printf("ERROR! Incorrect create_array!");
            break;
        }

        if(strcmp(token, "create_array") == 0)
        {
            array_size = create_array_handler(token, &array);
            if(array_size == -1 || array_size == 0)
            {
                printf("ERROR! Incorrect create_array!");
                break;
            }
        }
        else if(strcmp(token, "wc_files") == 0)
        {
            wc_files_handler(token);
        }
        else if(strcmp(token, "create_and_add_block\n") == 0 || strcmp(token, "create_and_add_block") == 0)
        {
            int block_index = create_and_add_block_handler(result, array, array_size);
            printf("Block created at index: %d\n", block_index);
        }
        else if(strcmp(token, "delete_block") == 0)
        {
            delete_block_handler(token, array);
            if(array_size == -1)
            {
                printf("ERROR! Incorrect create_array!");
                break;
            }
        }
        else if(strcmp(token, "view_array\n") == 0)
        {
            view_array(array, array_size);
        }
        else
        {
            printf("ERROR! A command %s is not recognised!", token);
            break;
        }
        
        end_clock();
    }
    #ifdef DYNAMIC_LOADING
        dlclose(handle);
    #endif
    return 0;

}