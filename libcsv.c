#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcsv.h"

#define MAX_COLUMNS 256
#define MAX_FILTERS 256

typedef struct {
    char header[256];
    char op;
    char value[256]; // Alteração para suportar valor como string
} Filter;

void printSelectedHeaders(const char *selectedColumns, char *headers[], int headerCount) {
    if (strlen(selectedColumns) > 0) {
        char *columnsCopy = strdup(selectedColumns);
        char *col = strtok(columnsCopy, ",");
        int first = 1;
        while (col) {
            for (int i = 0; i < headerCount; i++) {
                if (strcmp(col, headers[i]) == 0) {
                    if (!first) printf(",");
                    printf("%s", headers[i]);
                    first = 0;
                    break;
                }
            }
            col = strtok(NULL, ",");
        }
        free(columnsCopy);
    } else {
        for (int i = 0; i < headerCount; i++) {
            if (i > 0) printf(",");
            printf("%s", headers[i]);
        }
    }
    printf("\n");
}

int parseFilters(const char *rowFilterDefinitions, Filter *filters, char *headers[], int headerCount) {
    int filterCount = 0;
    if (strlen(rowFilterDefinitions) > 0) {
        char *filtersCopy = strdup(rowFilterDefinitions);
        char *filterLine = strtok(filtersCopy, "\n");
        while (filterLine) {
            char *op = strpbrk(filterLine, "><=!");
            if (op) {
                int opIndex = (int)(op - filterLine);
                if (opIndex > 0) {
                    strncpy(filters[filterCount].header, filterLine, opIndex);
                    filters[filterCount].header[opIndex] = '\0';

                    filters[filterCount].op = filterLine[opIndex];
                    strcpy(filters[filterCount].value, filterLine + opIndex + 1); // Copia valor como string

                    // Validate header
                    int validHeader = 0;
                    for (int i = 0; i < headerCount; i++) {
                        if (strcmp(filters[filterCount].header, headers[i]) == 0) {
                            validHeader = 1;
                            break;
                        }
                    }

                    if (!validHeader) {
                        fprintf(stderr, "Header '%s' not found in CSV\n", filters[filterCount].header);
                        free(filtersCopy);
                        return -1;
                    }

                    filterCount++;
                }
            }
            filterLine = strtok(NULL, "\n");
        }
        free(filtersCopy);
    }
    return filterCount;
}

int applyFilters(const char *csvLine, const Filter *filters, int filterCount, char *headers[], int headerCount) {
    char *lineCopy = strdup(csvLine);
    char *value = strtok(lineCopy, ",");
    int columnIndex = 0;

    while (value) {
        if (columnIndex < headerCount) {
            for (int i = 0; i < filterCount; i++) {
                if (strcmp(headers[columnIndex], filters[i].header) == 0) {
                    switch (filters[i].op) {
                        case '>':
                            if (!(strcmp(value, filters[i].value) > 0)) { // Comparação de string
                                free(lineCopy);
                                return 0;
                            }
                            break;
                        case '<':
                            if (!(strcmp(value, filters[i].value) < 0)) { // Comparação de string
                                free(lineCopy);
                                return 0;
                            }
                            break;
                        case '=':
                            if (strcmp(value, filters[i].value) != 0) { // Comparação de string
                                free(lineCopy);
                                return 0;
                            }
                            break;
                        // Adicione outros casos de filtro aqui se necessário
                        default:
                            fprintf(stderr, "Invalid comparison operator '%c' for string comparison.\n", filters[i].op);
                            free(lineCopy);
                            return 0;
                    }
                    break;
                }
            }
            value = strtok(NULL, ",");
            columnIndex++;
        } else {
            break;
        }
    }
    free(lineCopy);
    return 1;
}

void processCsv(const char* csv, const char* selectedColumns, const char* rowFilterDefinitions) {
    // Parse CSV data
    char *csvCopy = strdup(csv);
    char *line = strtok(csvCopy, "\n");
    char *headers[MAX_COLUMNS];
    int headerCount = 0;

    if (line) {
        char *header = strtok(line, ",");
        while (header) {
            headers[headerCount++] = strdup(header);
            header = strtok(NULL, ",");
        }
    }

    // Parse filters
    Filter filters[MAX_FILTERS];
    int filterCount = parseFilters(rowFilterDefinitions, filters, headers, headerCount);

    // Print selected headers
    printSelectedHeaders(selectedColumns, headers, headerCount);

    // Process CSV lines
    while ((line = strtok(NULL, "\n"))) {
        if (applyFilters(line, filters, filterCount, headers, headerCount)) {
            char *lineCopy = strdup(line);
            char *value = strtok(lineCopy, ",");
            int columnIndex = 0;
            int first = 1;

            while (value) {
                if (columnIndex < headerCount) {
                    if (strlen(selectedColumns) == 0 || strstr(selectedColumns, headers[columnIndex]) != NULL) {
                        if (!first) printf(",");
                        printf("%s", value);
                        first = 0;
                    }
                    value = strtok(NULL, ",");
                    columnIndex++;
                } else {
                    break;
                }
            }
            free(lineCopy);
            printf("\n");
        }
    }

    for (int i = 0; i < headerCount; i++) {
        free(headers[i]);
    }
    free(csvCopy);
}
