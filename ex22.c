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
#include <sys/wait.h>
#define ROWS 3
#define COLUMNS 151


// This function uses getcwd to return the full path to a.out
void getaOut(char compPath[]) {
    getcwd(compPath, 2 * COLUMNS);
    char a[] ="/a.out";
    strcat(compPath, a);
}

// This function gets 2D array and put null in each character of the array.
void nullify(char arr[ROWS][COLUMNS]) {
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            arr[i][j] = 0;
        }
    }
}


// This function reads a line from the file descriptor.
int readALine(char configurations[ROWS][COLUMNS], int fileFd, int lineNum) {
    char b = '\0';
    int numOfRead = 0;
    numOfRead = read(fileFd, &b, 1);
    if(numOfRead == -1) {
        write(1, "Error in: read\n", 16);
        exit(-1);
    }
    int readBytes = 0;
    while (b != '\n' && b != EOF) {
        readBytes++;
        configurations[lineNum][readBytes - 1] = b;
        numOfRead = read(fileFd, &b, 1);
        if(numOfRead == -1) {
            write(1, "Error in: read\n", 16);
            exit(-1);
        }
    }
    configurations[lineNum][readBytes] = '\0';
    return readBytes;
}


// This function reads the file line by line.
void readFile(char configurations[ROWS][COLUMNS], int fileFd) {
    for(int i = 0; i < 3; i++) {
        int readBytes = readALine(configurations, fileFd, i);
        if(readBytes == 0) {
            exit(-1);
        }
    }

}


// This function opens the directory using opendir function.
DIR* isDir(char* path) {
    DIR* dir = opendir(path);
    if(dir == NULL) {
        write(1, "Not a valid directory\n", 23);
        exit(-1);
    }
    return dir;
}

//This function checks that there are enough arguments in argv and opens the file
// according to the path from the user's arguments.
void useArguments(int argc, char* argv[], char configurations[ROWS][COLUMNS]) {
    if (argc < 2) {
        write(1, "Not enough arguments\n", 22);
        exit(-1);
    }
    char* path = argv[1];
    int fileFd = open(path, O_RDONLY);
    readFile(configurations, fileFd);
    int isClose = close(fileFd);
    if(isClose == -1) {
        write(1, "Error in: close\n", 17);
        exit(-1);
    }

}

// This function opens the input text file
int checkInputTxt(char* input) {
    int inputTxt = open(input, O_RDONLY);
    if(inputTxt == -1) {
        write(1,  "Input file not exist\n", 22);
        exit(-1);
    }
    return inputTxt;
}

// This function opens the output text file
int checkOutputTxt(char* input) {
    int outputTxt = open(input, O_RDONLY);
    if(outputTxt == -1) {
        write(1,  "Output file not exist\n", 23);
        exit(-1);
    }
    return outputTxt;
}



// This function gets a file name and checks if it is a C file.
int isCFile(char* dName) {
    int size = strlen(dName);
    if(size >= 2) {
        if(dName[size - 2] == '.' && dName[size - 1] == 'c') {
            return 1;
        }
    }
    return 0;
}

// This function redirects fd 2 to errors.txt
void redirectError(int errorsFd) {
    int stdError = dup(2);
    // checks that dup succeeded
    if(stdError == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
    if(close(2) == -1) {
        write(1, "Error in: close\n", 17);
        exit(-1);
    }
    // insert InputTxt to fd 0 for redirection
    if(dup(errorsFd) == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
}

// This function gets a C file and compiles it, using execvp func for calling gcc.
int compile(char* dName, int inputTxt, int outputTxt, int errorsFd) {
    int status = 0;
    // initiates child process.
    pid_t pid = fork();
    // child process
    if (pid == 0) {
        redirectError(errorsFd);
        char* args[] = {"gcc", dName, NULL};
        execvp(args[0], args);
        write(1, "Error in: execvp\n",18);
        exit(-1);
    }
    // parent process
    else if (pid > 0) {
        // waits for the child process to terminate
        pid = waitpid(pid, &status, 0);
        if (pid == -1) {
            write(1, "Error in: waitpid\n", 19);
        }
    }
    // fork failed
    else {
        write(1, "Error in: fork\n", 16);
    }
    return status;
}


// This function redirects the basic file descriptors 0,1,2 to the files.
void redirect(int inputTxt, int errorsFd) {
    int stdIn = dup(0);
    // checks that dup succeeded
    if(stdIn == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
    if(close(0) == -1) {
        write(1, "Error in: close\n", 17);
        exit(-1);
    }
    // insert InputTxt to fd 0 for redirection
    if(dup(inputTxt) == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
    int outputFd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(outputFd == -1) {
        write(1, "Error in: open\n", 16);
        return;
    }
    int stdOut = dup(1);
    if(stdOut == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
    if(close(1) == -1) {
        write(1, "Error in: close\n", 17);
        exit(-1);
    }
    // insert outputTxt to fd 1 for redirection
    if(dup(outputFd) == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }

    int stdError = dup(2);
    // checks that dup succeeded
    if(stdError == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
    if(close(2) == -1) {
        write(1, "Error in: close\n", 17);
        exit(-1);
    }
    // insert InputTxt to fd 0 for redirection
    if(dup(errorsFd) == -1) {
        write(1, "Error in: dup\n", 15);
        exit(-1);
    }
}



// This function runs a.out using execvp.
int secondPhase(int inputTxt, int outputTxt, int errorsFd) {
    int isFive = 0;
    int status;
    time_t start, end;
    time(&start);
    // initiates child process.
    pid_t pid = fork();
    // child process
    if (pid == 0) {
        redirect(inputTxt, errorsFd);
        // move to the beginning of the file.
        if(lseek(inputTxt, 0, SEEK_SET) == -1) {
            write(1, "Error in: lseek\n", 17);
        }
        char a[2 * COLUMNS] = {0};
        getaOut(a);
        char* args[] = {"./a.out", a, NULL};
        execvp(args[0], args);
        write(1, "Error in: execvp\n",18);
        exit(1);
    }
        // parent process
    else if (pid > 0) {
        // waits for the child process to terminate
        pid = waitpid(pid, &status, 0);
        if (pid == -1) {
            write(1, "Error in: waitpid\n",19);
            exit(-1);
        }
        time(&end);
        double diff = difftime(end, start);

        if(diff > 5.0) {
            isFive = 1;
        }
    }
        // fork failed
    else {
        write(1, "Error in: fork\n",16);
        exit(-1);
    }
    return isFive;
}

// This function writes to result.csv the grade for each directory.
void writeToResult(int status, char* name,int resultFd, int isC, int isCompile, int isFive) {
    if(isC == 0) {
        int size = strlen(name);
        char temp[size + 13];
        strcpy(temp,name);
        char a[] = ",0,NO_C_FILE\n";
        strcat(temp, a);
        if(write(resultFd, temp, size + 13) == -1 ) {
            return;
        }
        return;
    }
    if(isCompile == 256) {
        int size = strlen(name);
        char a[] = ",10,COMPILATION_ERROR\n";
        int size2 = strlen(a);
        char temp[size + size2];
        strcpy(temp, name);
        strcat(temp, a);
        write(resultFd, temp, size + size2);
        return;
    }
    if(isFive) {
        int size = strlen(name);
        char a[] = ",20,TIMEOUT\n";
        int size2 = strlen(a);
        char temp[size + size2];
        strcpy(temp, name);
        strcat(temp, a);
        write(resultFd, temp, size + size2);
        return;
    }
    if(status == 2) {
        int size = strlen(name);
        char a[] = ",50,WRONG\n";
        int size2 = strlen(a);
        char temp[size + size2];
        strcpy(temp, name);
        strcat(temp, a);
        write(resultFd, temp, size + size2);
        return;
    }
    if(status == 3) {
        int size = strlen(name);
        char a[] = ",75,SIMILAR\n";
        int size2 = strlen(a);
        char temp[size + size2];
        strcpy(temp, name);
        strcat(temp, a);
        write(resultFd, temp, size + size2);
        return;
    }
    if(status == 1) {
        int size = strlen(name);
        char a[] = ",100,EXCELLENT\n";
        int size2 = strlen(a);
        char temp[size + size2];
        strcpy(temp, name);
        strcat(temp, a);
        write(resultFd, temp, size + size2);
        return;
    }
}

// This function runs comp.out and using it to give grade for each directory.
int partA(char fullPath[], char* compPath, int errorsFd) {
    int status = 0;
    // initiates child process.
    pid_t pid = fork();
    // child process
    if (pid == 0) {
        execl(compPath, "comp.out", fullPath, "./output.txt", NULL);
        write(1, "Error in: execl\n",17);
        exit(-1);
    }
        // parent process
    else if (pid > 0) {
        // waits for the child process to terminate
        pid = waitpid(pid, &status, 0);
        if (pid == -1) {
            write(1, "Error in: waitpid\n",19);
            exit(-1);
        }
    }
        // fork failed
    else {
        write(1, "Error in: fork\n",16);
        exit(-1);
    }
    return (status >> 8);
}



// This function removes the file "a.out" from the current directory.
void removeOut() {
    int status = 0;
    // initiates child process.
    pid_t pid = fork();
    // child process
    if (pid == 0) {
        char* args[] = {"rm", "a.out", NULL};
        execvp(args[0], args);
        write(1, "Error in: execvp\n",18);
        exit(-1);
    }
        // parent process
    else if (pid > 0) {
        // waits for the child process to terminate
        pid = waitpid(pid, &status, 0);
        if (pid == -1) {
            write(1, "Error in: waitpid\n",19);
            exit(-1);
        }
    }
        // fork failed
    else {
        write(1, "Error in: fork\n",16);
        exit(-1);
    }
}

// This function removes the file "output.txt" from the current directory.
void removeOutput() {
    int status = 0;
    // initiates child process.
    pid_t pid = fork();
    // child process
    if (pid == 0) {
        char* args[] = {"rm", "output.txt", NULL};
        execvp(args[0], args);
        write(1, "Error in: execvp\n",18);
        exit(-1);
    }
        // parent process
    else if (pid > 0) {
        // waits for the child process to terminate
        pid = waitpid(pid, &status, 0);
        if (pid == -1) {
            write(1, "Error in: waitpid\n",19);
        }
    }
        // fork failed
    else {
        write(1, "Error in: fork\n",16);
        exit(-1);
    }
}




// This function removes files from the current directory.
void removeFiles(int isC, int isCompile) {
    if(isC) {
        removeOutput();
        if(!isCompile) {
            removeOut();
        }
    }
}


// This function iterates over the entries in the directory and handles each file differently.
void proceed(DIR* dir, int inputTxt, int outputTxt, int errorsFd, char fullPath[], char* dNameStart, int resultFd
                , char* compPath){
    int isFive = 0;
    int status = 0;
    int isC = 0;
    int isCompile = 0;
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL) {
        if(isCFile(entry->d_name) && entry->d_type != DT_DIR) {
            isC = 1;
            isCompile = compile(entry->d_name, inputTxt, outputTxt, errorsFd);
            isFive = secondPhase(inputTxt, outputTxt, errorsFd);
            status = partA(fullPath, compPath, errorsFd);
            if(status == 255) {
                removeFiles(isC, isCompile);
                return;
            }
        }
    }
    writeToResult(status, dNameStart, resultFd, isC, isCompile, isFive);
    removeFiles(isC, isCompile);
}


// This function opens the directory and iterate over the entries.
void handle(char* dNAme, int inputTxt, int outputTxt, int errorsFd, char fullPath[], int resultFd, char* compPath) {
    DIR* dir = opendir(dNAme);
    // checks if the opendir succeeded
    if(dir == NULL) {
        write(1, "Error in: opendir\n", 19);
        exit(-1);
    }
    //changes the dir to the user's folder
    int change = chdir(dNAme);
    // checks if the chdir succeeded
    if(change == -1) {
        write(1, "Error in: chdir\n", 17);
        int close = closedir(dir);
        if(close == -1) {
            write(1, "Error in: closedir\n", 20);
        }
        return;
    }
    proceed(dir, inputTxt, outputTxt, errorsFd, fullPath, dNAme, resultFd, compPath);
    // close the directory
    if(closedir(dir) == -1) {
        write(1, "Error in: closedir\n", 20);
    }
    // returns to all the user's directory from a specific user's folder
    if(chdir("..") == -1) {
        write(1, "Error in: chdir\n", 17);
        exit(-1);
    }
}


// This function runs the main logic of the program.
void run(DIR* dir, int inputTxt, int outputTxt, char configurations[ROWS][COLUMNS], char fullPath[], int resultFd
            ,char* compPath ) {
    char current[COLUMNS] = {0};
    getcwd(current, COLUMNS);

    int errorsFd = open("errors.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(errorsFd == -1) {
        write(1, "Error in: open\n", 16);
        return;
    }
    int change = chdir(configurations[0]);
    if(change == -1) {
        write(1, "Not a valid directory\n", 23);
        exit(-1);
    }
    struct dirent* entry;
    // iterates over the entries in the directory.
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // checks if the entry is a directory.
            if(entry->d_type == DT_DIR) {
                handle(entry->d_name, inputTxt, outputTxt, errorsFd, fullPath, resultFd, compPath);
            }
        }
    }


}

// This function creates a char* that contains the path to the file.
void getPath(char fullPath[], char* more) {
    getcwd(fullPath, COLUMNS);
    int size = strlen(fullPath);
    int moreSize = strlen(more);
    fullPath[size] = '/';
    for(int i = 0; i < moreSize; i++) {
        fullPath[size + i + 1] = more[i];
    }
    return;
}

// This function opens the file result.csv, it creates it if not exist, or opens it otherwise.
int getResult() {
    int temp = open("results.csv", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(temp == -1) {
        write(1, "Error in: open\n", 16);
        exit(-1);
    }
    return temp;
}

// This function creates a char* that contains the path to comp.out
void getCompOut(char compPath[]) {
    getcwd(compPath, 2 * COLUMNS);
    char a[] ="/comp.out";
    strcat(compPath, a);
}


//This function closes a specific file descriptor.
void closeOne(int fd) {
    int check = close(fd);
    if(check == -1) {
        write(1, "Error in: close\n",17);
        exit(-1);
    }
}

// This function closes the files descriptor that were opened in the program.
void closeFiles(int inputTxt, int outputTxt) {
    closeOne(inputTxt);
    closeOne(outputTxt);
}


//This is the main function of the program.
int main(int argc, char* argv[]) {
    char configurations[ROWS][COLUMNS];
    nullify(configurations);
    useArguments(argc, argv, configurations);
    char fullPath[2 * COLUMNS] = {0};
    getPath(fullPath, configurations[2]);
    char compPath[2 * COLUMNS] = {0};
    getCompOut(compPath);
    int resultFd = getResult();
    DIR* dir = isDir(configurations[0]);
    int inputTxt = checkInputTxt(configurations[1]);
    int outputTxt = checkOutputTxt(configurations[2]);
    run(dir, inputTxt, outputTxt, configurations, fullPath, resultFd, compPath);
    closeFiles(inputTxt, outputTxt);
    return 0;
}
