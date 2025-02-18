/*
finds all the files in a directory tree with a specific name
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

/* matches filename with pattern */
int matchPattern(char *path, char *pattern) {
    int n = strlen(path);
    for (int i = strlen(pattern); i >= 0; ) {
        if (path[n--] != pattern[i--]) return 0;
    }
    if (path[n] != '/') return 0;
    return 1;
}

/* test if path is /. or /.. */
int isSelfOrParentDir(char *path) {
    int n = strlen(path);
    if (path[n-2] == '/' && path[n-1] == '.') return 1;
    if (path[n-3] == '/' && path[n-2] == '.' && path[n-1] == '.') return 1;
    return 0;
}

/* recursive function for find */
void findRecurse(char *path, char *pattern) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // test if path is valid
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // test if fstat returns error
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return;
    }

    // test first argument is directory or not
    if (st.type != T_DIR) {
        fprintf(2, "find: first argument has to be directory\n");
        close(fd);
        return;
    }

    // test if path will be bigger thatn buffer size
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }
    
    // copy path to buffer
    strcpy(buf, path);

    // set pointer to &char[strlen(buf)] and append /
    p = buf + strlen(buf);
    *p++ = '/';

    // iterate through the directory
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0) continue;

        // append directory entry name and add terminator
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // test if fstat returns error
        if (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
        }
        
        // print if file & matches
        if (st.type == T_FILE && matchPattern(buf, pattern)) printf("%s\n", buf);
        
        // recurse if dir & not refering to self or parent
        else if (st.type == T_DIR && !isSelfOrParentDir(buf)) findRecurse(buf, pattern);
    }
    return;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        char message[] = "find: argument error\n";
        write(1, message, strlen(message));
        exit(0);
    }
    findRecurse(argv[1], argv[2]);
    exit(0);
}