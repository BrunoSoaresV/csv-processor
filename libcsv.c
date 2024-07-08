#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcsv.h"

#define MAX_COLUMNS 256
#define MAX_FILTERS 256

typedef struct {
    char header[256];
    char op;
    char value[256];
} Filter;

void printSelectedHeaders(const char *selectedColumns, char *headers[], int headerCount) {
    if (strlen(selectedColumns) > 0) {
        char *columnsCopy = strdup(selectedColumns);
        if (!columnsCopy) {
            fprintf(stderr, "Erro de alocação de memória\n");
            return;
        }
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
        if (!filtersCopy) {
            fprintf(stderr, "Erro de alocação de memória\n");
            return -1;
        }
        char *filterLine = strtok(filtersCopy, "\n");
        while (filterLine) {
            char *op = strpbrk(filterLine, "><=!");
            if (op) {
                int opIndex = (int)(op - filterLine);
                if (opIndex > 0) {
                    strncpy(filters[filterCount].header, filterLine, opIndex);
                    filters[filterCount].header[opIndex] = '\0';

                    filters[filterCount].op = filterLine[opIndex];
                    strcpy(filters[filterCount].value, filterLine + opIndex + 1);

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
    if (!lineCopy) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 0;
    }
    char *value = strtok(lineCopy, ",");
    int columnIndex = 0;
    int passFilter = 1;

    while (value) {
        if (columnIndex < headerCount) {
            for (int i = 0; i < filterCount; i++) {
                if (strcmp(headers[columnIndex], filters[i].header) == 0) {
                    switch (filters[i].op) {
                        case '>':
                            if (!(strcmp(value, filters[i].value) > 0)) {
                                passFilter = 0;
                            }
                            break;
                        case '<':
                            if (!(strcmp(value, filters[i].value) < 0)) {
                                passFilter = 0;
                            }
                            break;
                        case '=':
                            if (strcmp(value, filters[i].value) != 0) {
                                passFilter = 0;
                            }
                            break;
                        default:
                            fprintf(stderr, "Invalid comparison operator '%c' for string comparison.\n", filters[i].op);
                            free(lineCopy);
                            return 0;
                    }
                    break;
                }
            }

            if (!passFilter) {
                free(lineCopy);
                return 0;
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
    char *csvCopy = strdup(csv);
    if (!csvCopy) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return;
    }
    char *line = strtok(csvCopy, "\n");
    char *headers[MAX_COLUMNS];
    int headerCount = 0;

    if (line) {
        char *header = strtok(line, ",");
        while (header) {
            headers[headerCount++] = strdup(header);
            if (!headers[headerCount - 1]) {
                fprintf(stderr, "Erro de alocação de memória\n");
                free(csvCopy);
                return;
            }
            header = strtok(NULL, ",");
        }
    }

    Filter filters[MAX_FILTERS];
    int filterCount = parseFilters(rowFilterDefinitions, filters, headers, headerCount);

    if (filterCount == -1) {
        free(csvCopy);
        return;
    }

    printSelectedHeaders(selectedColumns, headers, headerCount);

    while ((line = strtok(NULL, "\n"))) {
        if (applyFilters(line, filters, filterCount, headers, headerCount)) {
            char *lineCopy = strdup(line);
            if (!lineCopy) {
                fprintf(stderr, "Erro de alocação de memória\n");
                free(csvCopy);
                return;
            }
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

void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    FILE *file = fopen(csvFilePath, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", csvFilePath);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *csv = malloc(fileSize + 1);
    if (!csv) {
        fclose(file);
        fprintf(stderr, "Erro de alocação de memória\n");
        return;
    }

    fread(csv, 1, fileSize, file);
    csv[fileSize] = '\0';
    fclose(file);

    processCsv(csv, selectedColumns, rowFilterDefinitions);

    free(csv);
}
