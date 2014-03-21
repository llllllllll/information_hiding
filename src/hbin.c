// Joe Jevnik
// 2014.2.19
// Reads hidden data written with hwrite.

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define TMP_FILE "/tmp/hbin_binary"

int main(int argc,char **argv){
  int fd,tfd;
    char *file,*bin;
    struct stat s;
    if (argc == 1){
        puts("Usage: hread FILE");
        return EXIT_SUCCESS;
    }
    if ((fd = open(argv[1],O_RDWR)) == -1){
        perror("open argv[1]");
        return EXIT_FAILURE;
    }
    if ((tfd = open(TMP_FILE,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
        perror("open TMP_FILE");
        return EXIT_FAILURE;
    }
    stat(argv[1],&s);
    file = mmap(NULL,s.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    bin  = mmap(NULL,s.st_blksize,PROT_READ | PROT_WRITE,MAP_SHARED,tfd,0);
    memcpy(bin,file + s.st_size + 1,(!(s.st_size % s.st_blksize)) ? 0
           : s.st_blksize - (s.st_size % s.st_blksize) - 1);
    memmove(argv,argv + 1,argc - 1);
    argv[argc - 1] = NULL;
    execv(TMP_FILE,argv);
    return EXIT_FAILURE;
}
