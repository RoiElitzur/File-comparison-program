//Roi Elitzur 206780652
#include <stdio.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int openFile(char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        write(1, "Error in: open\n", 16);
        exit(-1);
    }
    return fd;
}


void checkArguments(int argc) {
    if(argc < 3) {
        exit(-1);
    }
}


int isIdentical(int firstFd, int secondFd) {
    char a = '\0';
    char b = '\0';
    int first = 1;
    int second = 1;
    while(first != 0 && second != 0) {
        first = read(firstFd, &a, 1);
        if(first == -1) {
            write(1, "Error in: read\n", 16);
        }
        second = read(secondFd, &b, 1);
        if(second == -1) {
            write(1, "Error in: read\n", 16);
        }
        if(a != b) {
            return 0;
        }
    }
    if(first == second) {
        return 1;
    }
    return 0;
}

char readChar(int fd, int* readBytes) {
    char c = '\0';
    *readBytes = read(fd, &c, 1);
    while((*readBytes != 0)) {
        if(c != '\n' && c != ' ') {
            if(c >= 65 && c <= 90) {
                return (c + 32);
            }
            return c;
        }
        *readBytes = read(fd, &c, 1);
    }
    return 'A';
}

void takeBack(int fd) {
    int check = lseek(fd, 0 , SEEK_SET);
    if(check == -1) {
        write(1, "Error in: lseek\n", 17);
        exit(-1);
    }
}


int isSimilar(int firstFd, int secondFd) {
    takeBack(firstFd);
    takeBack(secondFd);
    char a = '\0';
    char b = '\0';
    int first = 1;
    int second = 1;
    while(first != 0 && second != 0) {
        a = readChar(firstFd, &first);
        b = readChar(secondFd, &second);
        if(a != b) {
            return 0;
        }
    }
    return 1;
}

void closeFile(int fd) {
    int check = close(fd);
    if(check == -1) {
        write(1, "Error in: close\n", 17);
        exit(-1);
    }
}


int main(int argc, char* argv[]) {
    checkArguments(argc);
    int firstFd = openFile(argv[1]);
    int secondFd = openFile(argv[2]);
    int checkIdentical =  isIdentical(firstFd, secondFd);
    if(checkIdentical == 1) {
        closeFile(firstFd);
        closeFile(secondFd);
        exit(1);
    }
    int checkSimilar = isSimilar(firstFd, secondFd);
    if(checkSimilar == 1) {
        closeFile(firstFd);
        closeFile(secondFd);
        exit(3);
    }
    closeFile(firstFd);
    closeFile(secondFd);
    exit(2);
}
