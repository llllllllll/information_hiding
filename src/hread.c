/* hread.c --- A program to read strings hidden in a normal file that were
               written with hwrite.
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
    close(fd);
    return EXIT_SUCCESS;
}
