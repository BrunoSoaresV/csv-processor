#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "libcsv.h"

// Helper function to capture stdout
char* captureStdout(void (*func)(const char[], const char[], const char[]), const char csv[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    fflush(stdout);
    int old_stdout = dup(STDOUT_FILENO);
    int pipefd[2];
    pipe(pipefd);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    func(csv, selectedColumns, rowFilterDefinitions);

    fflush(stdout);
    dup2(old_stdout, STDOUT_FILENO);
    close(old_stdout);

    static char buffer[4096];
    int bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';

    return buffer;
}

// Test function for processCsv
void testProcessCsv() {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char filters[] = "header1>1\nheader3<8";
    printf("Original CSV:\n%s\n", csv);
    char* output = captureStdout(processCsv, csv, selectedColumns, filters);
    printf("Processed CSV:\n%s", output);
}

int main() {
    testProcessCsv();
    return 0;
}
