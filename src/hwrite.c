/* hwrite.c --- A program to write a list of null terminated strings to the
                unused portion of the last block.
   Copyright (c) 2014 Joe Jevnik

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 51
   Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

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
    FILE *f;
    size_t textl,hiddenl,argoff = 0;
    off_t ind;
    char *file;
    blksize_t page;
    struct stat s;
    if (argc < 4){
        puts("Usage: hwrite [-b] FILE VISIBLE HIDDEN...");
        return EXIT_SUCCESS;
    }
    if (!strncmp(argv[1],"-b",2)){
        argoff = 1;
    }
    if (!access(argv[1 + argoff],F_OK)){
        fprintf(stderr,"File '%s' already exists!\n",argv[1 + argoff]);
        return EXIT_FAILURE;
    }
    if (!strlen(argv[2 + argoff])){
        fputs("You must provide a non-null string to write as visible data!\n",
              stderr);
        return EXIT_FAILURE;
    }
    if ((fd = open(argv[1 + argoff],O_RDWR | O_CREAT, S_IRUSR | S_IWUSR))
        == -1){
        perror("open");
        return EXIT_FAILURE;
    }
    stat(argv[1 + argoff],&s);
    page = s.st_blksize;
    ind = textl = strlen(argv[2 + argoff]);
    write(fd,argv[2 + argoff],textl);
    file = mmap(NULL,textl,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if (argoff){
        for (n = 3 + argoff;n < argc;n++){
            stat(argv[n],&s);
            if ((ind % page) + s.st_size + 1 > page){
                goto OVERFLOW;
            }
            fread(&file[ind],sizeof(char),s.st_size,f = fopen(argv[n],"r"));
            fclose(f);
            ind += s.st_size;
        }
    }else{
        file[ind] = (unsigned char) (argc - (3 + argoff));
        ind += sizeof(unsigned char);
        for (n = 3 + argoff;n < argc - argoff;n++){
            hiddenl = strlen(argv[n]);
            if ((ind % page) + hiddenl + 1 > page){
                goto OVERFLOW;
            }
            memcpy(&file[ind],argv[n + argoff],hiddenl);
            file[ind + hiddenl] = 0;
            ind += hiddenl + 1;
        }
    }
    munmap(file,textl);
    close(fd);
    return EXIT_SUCCESS;

// Cleanup in case of overflow
OVERFLOW:
    fputs("Data exceeds the page size!\n",stderr);
    munmap(file,textl);
    unlink(argv[1 + argoff]);
    return EXIT_FAILURE;
}
