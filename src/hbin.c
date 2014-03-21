/* hbin.c --- A program to read an execute binary files hidden in a normal file
              that was written with hwrite -b.
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
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define TMP_FILE "/tmp/hbin_binary"

int main(int argc,char **argv){
    int fd,tfd,status;
    char *file;
    struct stat s;
    if (argc == 1){
        puts("Usage: hbin FILE");
        return EXIT_SUCCESS;
    }
    if ((fd = open(argv[1],O_RDWR)) == -1){
        perror("open argv[1]");
        return EXIT_FAILURE;
    }
    if ((tfd = open(TMP_FILE,O_RDWR | O_CREAT,
                    S_IRUSR | S_IWUSR | S_IXUSR)) == -1){
        perror("open TMP_FILE");
        return EXIT_FAILURE;
    }
    stat(argv[1],&s);
    file = mmap(NULL,s.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    write(tfd,file + s.st_size,(s.st_blksize - s.st_size) % s.st_blksize);
    memmove(argv,&argv[1],argc);
    argv[argc - 1] = NULL;
    close(tfd);
    munmap(file,s.st_size);
    switch(fork()){
    case -1:
        perror("fork");
        break;
    case 0:
        execv(TMP_FILE,argv);
        return EXIT_FAILURE;
    default:
        wait(&status);
        if (!WIFEXITED(status)){
            fputs("Failed to run the program\n",stderr);
        }
        unlink(TMP_FILE);
    }
    return EXIT_SUCCESS;
}
