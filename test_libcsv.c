#include <stdio.h>
#include "libcsv.h"

int main() {
    const char selectedColumns[] = "col1,col3";
    const char rowFilterDefinitions[] = "col1>l1c1";

    printf("Original CSV:\n");
    processCsvFile("data.csv", "", "");

    printf("Processed CSV:\n");
    processCsvFile("data.csv", selectedColumns, rowFilterDefinitions);

    return 0;
}
