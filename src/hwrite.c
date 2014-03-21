// Joe Jevnik
// 2014.2.19
// Writes hidden and visible data to a file.
// Hidden data is written as a series of NULL terminated strings.

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

int main(int argc,char **argv){
    int fd,n;
    size_t textl,hiddenl;
    off_t ind;
    char *file;
    blksize_t page;
    struct stat s;
    if (argc < 4){
        puts("Usage: hwrite FILE VISIBLE HIDDEN...");
        return EXIT_SUCCESS;
    }
    if (!access(argv[1],F_OK)){
        fprintf(stderr,"File '%s' already exists!\n",argv[1]);
        return EXIT_FAILURE;
    }
    if (!strlen(argv[2])){
        fputs("You must provide a non-null string to write as visible data!\n",
              stderr);
        return EXIT_FAILURE;
    }
    if ((fd = open(argv[1],O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
        perror("open");
        return EXIT_FAILURE;
    }
    stat(argv[1],&s);
    page = s.st_blksize;
    ind = textl = strlen(argv[2]);
    write(fd,argv[2],textl);
    file = mmap(NULL,textl,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    file[ind] = (unsigned char) (argc - 3);
    ind += sizeof(unsigned char);
    for (n = 3;n < argc;n++){
        hiddenl = strlen(argv[n]);
        if (ind + hiddenl + 1 > page){
            fputs("Data exceeds the page size!\n",stderr);
            munmap(file,textl);
            unlink(argv[1]);
            return EXIT_FAILURE;
        }
        memcpy(&file[ind],argv[n],hiddenl);
        file[ind + hiddenl] = 0;
        ind += hiddenl + 1;
    }
    munmap(file,textl);
    return EXIT_SUCCESS;
}
