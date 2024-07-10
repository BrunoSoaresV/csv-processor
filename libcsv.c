#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcsv.h"

// Helper function to split a string by a delimiter
char **splitString(const char *str, const char *delimiter, int *count) {
    char *strCopy = strdup(str);
    char *token = strtok(strCopy, delimiter);
    int capacity = 10;
    char **result = malloc(capacity * sizeof(char *));
    int index = 0;

    while (token != NULL) {
        if (index >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(char *));
        }
        result[index++] = strdup(token);
        token = strtok(NULL, delimiter);
    }

    *count = index;
    free(strCopy);
    return result;
}

// Helper function to check if a row matches the filters
int rowMatchesFilters(char **row, char **headers, int numCols, char **filters, int numFilters) {
    for (int i = 0; i < numFilters; i++) {
        char *filter = strdup(filters[i]);
        char *header = strtok(filter, "=<>");
        char *value = filter + strlen(header) + 1;  // Skip the comparator
        char comparator = filters[i][strlen(header)];

        int colIndex = -1;
        for (int j = 0; j < numCols; j++) {
            if (strcmp(headers[j], header) == 0) {
                colIndex = j;
                break;
            }
        }

        if (colIndex == -1) {
            free(filter);
            return 0;  // Header not found
        }

        int cmp = strcmp(row[colIndex], value);
        free(filter);

        switch (comparator) {
            case '=':
                if (cmp != 0) return 0;
                break;
            case '>':
                if (cmp <= 0) return 0;
                break;
            case '<':
                if (cmp >= 0) return 0;
                break;
            default:
                return 0;
        }
    }
    return 1;
}

// Main processing function
void processCsv(const char csv[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    int numRows, numCols;
    char **rows = splitString(csv, "\n", &numRows);
    char **headers = splitString(rows[0], ",", &numCols);

    int numSelectedCols;
    char **selectedCols = splitString(selectedColumns, ",", &numSelectedCols);

    int numFilters;
    char **filters = splitString(rowFilterDefinitions, "\n", &numFilters);

    // Print the selected columns header
    for (int i = 0; i < numSelectedCols; i++) {
        if (i > 0) printf(",");
        printf("%s", selectedCols[i]);
    }
    printf("\n");

    // Process each row
    for (int i = 1; i < numRows; i++) {
        int rowNumCols;
        char **row = splitString(rows[i], ",", &rowNumCols);
        if (rowMatchesFilters(row, headers, numCols, filters, numFilters)) {
            for (int j = 0; j < numSelectedCols; j++) {
                if (j > 0) printf(",");
                int colIndex = -1;
                for (int k = 0; k < numCols; k++) {
                    if (strcmp(headers[k], selectedCols[j]) == 0) {
                        colIndex = k;
                        break;
                    }
                }
                if (colIndex != -1) printf("%s", row[colIndex]);
            }
            printf("\n");
        }
        for (int j = 0; j < rowNumCols; j++) free(row[j]);
        free(row);
    }

    // Free allocated memory
    for (int i = 0; i < numRows; i++) free(rows[i]);
    free(rows);
    for (int i = 0; i < numCols; i++) free(headers[i]);
    free(headers);
    for (int i = 0; i < numSelectedCols; i++) free(selectedCols[i]);
    free(selectedCols);
    for (int i = 0; i < numFilters; i++) free(filters[i]);
    free(filters);
}

void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    FILE *file = fopen(csvFilePath, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", csvFilePath);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *csv = malloc(fileSize + 1);
    fread(csv, 1, fileSize, file);
    csv[fileSize] = '\0';

    fclose(file);

    processCsv(csv, selectedColumns, rowFilterDefinitions);

    free(csv);
}
