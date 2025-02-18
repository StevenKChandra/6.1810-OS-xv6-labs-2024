/*
experiment to use fork() to create child process and pipe
*/
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    int pid = fork();

    if (pid == 0) {
        
        // *IMPORTANT*
        // make sure the write end of pipe is closed
        // otherwise (read(p[0], &pid, 1) > 0) will block
        // because it is waiting for next input
        close(p[1]);

        while (read(p[0], &pid, 1) > 0);
        printf("%d: received ping\n", pid);
        exit(0);
    }
    char buffer[1];
    *buffer = pid;
    write(p[1], buffer, 1);

    // *IMPORTANT* same here
    close(p[1]); 

    wait((int*) 0);
    pid = getpid();
    printf("%d: received pong\n", pid);
    exit(0);
}