#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float fun(float x)
{
    return 4/(x * x + 1);
}
/*
    Child process should get:
        * its ID
        * width of rectangle
        * number of rectangles
        * lower bound of integral
*/
int main(int argc, char ** argv)
{
    // handling args
    int id, n;
    float h, a;
    if(argc == 5)
    {
        id = atoi(argv[1]);
        h = atof(argv[2]);
        n = atoi(argv[3]);
        a = atof(argv[4]);
    }
    else
    {
        printf("Incorrect arguments! Pass h, n and b!");
        return 1;
    }

    // integrating
    float sum = 0;
    for(int i = 0; i < n; i++)
    {
        sum += fun(a + h * i) * h;
    }

    // getting filename
    char * filename = calloc(sizeof(char), 100);
    char * temp_buff = calloc(sizeof(char), 33);

    snprintf(temp_buff, 33, "%d", id);
    strcpy(filename, "data/w");
    strcat(filename, temp_buff);
    strcat(filename, ".txt");

    // creating file handle
    FILE * result_file = fopen(filename, "w");
    
    if(result_file == NULL)
    {   
        printf("Couldn't open input file: %s\n", filename);
        free(filename);
        free(temp_buff);
        return 1;
    }

    // saving results
    fprintf(result_file, "%f", sum);

    fclose(result_file);
    free(filename);
    free(temp_buff);

    return 0;
}