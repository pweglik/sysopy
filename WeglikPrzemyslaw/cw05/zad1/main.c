#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main() {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) { // dziecko
        close(fd[1]);
        char* buffer = calloc(sizeof(char), 101);
        read(fd[0], buffer, 100);
        execlp("touch", "touch", buffer, NULL);
    } else { // rodzic
        close(fd[0]);
        write(fd[1], "hello_world", 100);
    }

    return 0;
}