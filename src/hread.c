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

int main(int argc,char **argv){
    int fd;
    char *file;
    off_t ind;
    unsigned char c,n;
    struct stat s;
    if (argc == 1){
        puts("Usage: hread FILE");
        return EXIT_SUCCESS;
    }
    if ((fd = open(argv[1],O_RDWR)) == -1){
        perror("open");
        return EXIT_FAILURE;
    }
    stat(argv[1],&s);
    file = mmap(NULL,s.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    ind = lseek(fd,0,SEEK_END);
    c = file[ind];
    ind += sizeof(unsigned char);
    for (n = 0;n < c;n++){
        puts(&file[ind]);
        ind += strlen(&file[ind]) + 1;
    }
    munmap(file,s.st_size);
    return EXIT_SUCCESS;
}
