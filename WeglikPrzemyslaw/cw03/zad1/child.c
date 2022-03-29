#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid = getpid();
    printf("Process with pid %d is calling me!\n", pid);

    return 0;
}