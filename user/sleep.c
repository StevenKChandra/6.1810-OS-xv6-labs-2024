/*
makes system idle for some time proportional to input integer
*/
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int t = atoi(argv[1]);
    if (argc == 1) {
        char* error_message = "No argument passed for sleep\n";
        write(1, error_message, strlen(error_message));
    }
    else if (argc == 2) {
        sleep(t);
    }
    else {
        char* error_message = "Too many argument passed for sleep\n";
        write(1, error_message, strlen(error_message));
    }
    exit(0);
}