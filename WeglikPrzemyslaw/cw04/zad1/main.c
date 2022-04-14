#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int sig)
{
    printf("Correctly handled! Sig: %d\n", sig);
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("Wrong number of arguments!");
        return 1;
    }
    if(strcmp(argv[1], "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
    }
    else if(strcmp(argv[1], "handler") == 0)
    {
        signal(SIGUSR1, handler);
        raise(SIGUSR1);
    }
    else if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0)
            perror("Error while masking signal");

        if(strcmp(argv[1], "pending") == 0)
        {
            sigset_t current_signals;
            sigpending(&current_signals);
            printf("Signal pending in parent: %d\n", sigismember(&current_signals, SIGUSR1));
        }

        raise(SIGUSR1);
    }

    if(strcmp(argv[2], "exec") == 0)
    {
        execl("bin/child", "bin/child", argv[1], NULL);
    }
    else if(strcmp(argv[2], "fork") == 0)
    {
        pid_t child_pid = fork();
        if(child_pid == 0)
        {
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
        }
    }
    return 0;
}