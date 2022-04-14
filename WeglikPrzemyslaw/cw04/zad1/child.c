#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv)
{   
    if(argc != 2)
    {
        exit(-1);
    }
    if(strcmp(argv[1], "ignore") == 0)
    {
        raise(SIGUSR1);
        printf("Ignored in child \n");
    }
    else if(strcmp(argv[1], "mask") == 0)
    {
        raise(SIGUSR1);
        printf("Masked in children\n");
    }
    else if(strcmp(argv[1], "pending") == 0)
    {
        sigset_t current_signals;
		sigpending(&current_signals);
        printf("Signal pending in child: %d\n", sigismember(&current_signals, SIGUSR1));
    }
    return 0;
}