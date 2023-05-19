# Files Comparison Program
This repository contains a program that allows you to compare files and determine if they are identical, similar, or different. The program accepts paths to two files as arguments and returns a specific code based on the file comparison result.The program utilizes system calls such as open, close, read, write and exec.Each system call is meticulously checked for success to ensure the program's reliability.Additionally, the program performing file descriptor redirection using the renowned dup technique for input and output redirection.
####
# Guidelines for ex21.c
The program named ex21.c  determine whether two files are identical, similar, or different. Here are the rules for file comparison:
- Identical files contain exactly the same content. If the files are identical, the program returns 1.
- Similar files are not identical but contain the same text with differences in lowercase/uppercase letters, spaces, and/or line breaks. If the files are similar, the program returns 3.
- Different files are neither identical nor similar. If the files are different, the program returns 2.
 Please note the following:
 - The files may contain letters (uppercase or lowercase), numbers, spaces, or line breaks.
 # Usage Example
 1. To run the program and compare two files, first cpomile the files using the makefile and the then use the following command: ./comp.out <path_to_file1> <path_to_file2>
 2. After running the program, you can check the return value using the following command: echo $?
####
# Guidelines for ex22.c
The program c22.ex accepts a path to a configuration file as an argument. The configuration file should exist and contain the following three lines:
1. Path to the folder containing subfolders. Each subfolder represents a user and should contain a C file.
2. Path to the input file.
3. Path to the correct output file for the input.
# The program performs the following steps:
- Traverse the subfolders within the specified folder (ignoring non-folder files).
- In each subfolder, search for a C file and compile it.
- Run the compiled program with input redirection from the specified input file.
- Compare the program's output with the desired output using the out.comp program implemented in ex21.c.
- Generate a csv.results file in the program's folder, containing the username (subfolder name) and a score between 0 and 100 based on the comparison result.
# Possible reasons and corresponding scores:
- FILE_C_NO: No C file found in the user's folder. Score: 0.
- ERROR_COMPILATION: Compilation error (file does not compile). Score: 10.
- TIMEOUT: The compiled C file runs for more than 5 seconds. Score: 20.
- WRONG: The output is different from the desired output. Score: 50.
- SIMILAR: The output is different from the desired output but similar. Score: 75.
- EXCELLENT: The output is the same as the desired output. Score: 100.
