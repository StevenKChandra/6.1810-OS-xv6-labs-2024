/*
prints primes between 2 and 280 (because of xv6 resource limitation)
using fork() and pipe()
*/
#include "kernel/types.h"
#include "user/user.h"

void printPrime(int N) {
    char prime[] = "prime ";
    char numstring[8];

    int i = 0;

    // Extract digits from the number,
    // add them to the string
    while (N > 0) {
        numstring[i++] = N % 10 + '0';
      	N /= 10;
    }

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = numstring[j];
        numstring[j] = numstring[k];
        numstring[k] = temp;
    }

    // add endline and null-terminate the string
    numstring[i++] = '\n';
    numstring[i] = '\0';

    // write to stdout
    write(1, prime, strlen(prime));
    write(1, numstring, strlen(numstring));
}

int
main(int argc, char *argv[]) {
    // not reading from stderr
    close(2);

    // intial pipeline
    int p[2];
    pipe(p);

    int pid;
    int divider = 2;

    pid = fork();
    if (pid != 0){
        // PARENT PROCESS
        // not reading form stdin and read-end of pipe
        close(0);
        close(p[0]);

        printPrime(divider);

        // not writing stdout anymore
        close(1);

        // send next integers to child via write-end of pipeline
        for (int i = 3; i < 281; i++) {
            if (i % divider == 0) continue;
            write(p[1], &i, 4);
        }
    }
    else {
        // break after making a child process
        while (pid == 0) {

            // if this is process has no child
            if (pid == 0) {

                // not reading from grandparent's input
                close(0);
                // p[] is still PARENT's PIPE
                // duplicate read-end of pipe
                // because fd 0 was closed,
                // now the duplicate should be in fd table 0
                dup(p[0]);
                close(p[0]);

                // not writing to parent 
                close(p[1]);

                // replace divider, if divider is not replaced
                // and pipe is closed, then skip to end
                if (read(0, &divider, 4) == 0) {
                    break;
                }

                // create new pipe
                pipe(p);

                // create a child
                pid = fork();
                
                // if this is child, go to the begining of the loop
                if (pid == 0) continue;

                // parent not reading from pipe
                close(p[0]);

                printPrime(divider);

                // not writing to stdout anymore
                close(1);

                // pass number to child
                int temp;
                while (read(0, &temp, 4) > 0) {
                    if (temp % divider == 0) continue;
                    write(p[1], &temp, 4);
                }
            }
        }
    }
    if (pid != 0) {
        // *IMPORTANT*
        // signals no more write in pipeline
        // so child processes can exit
        close(p[1]);
        wait((int*) 0);
    }
    exit(0);
}