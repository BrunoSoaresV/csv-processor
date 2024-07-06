#include <stdio.h>
#include "libcsv.h"
#include <stdlib.h>

int main() {
    const char selectedColumns[] = "col1,col3";
    const char rowFilterDefinitions[] = "col1>l1c1";

    // Abre o arquivo data.csv para leitura
    FILE *file = fopen("data.csv", "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo data.csv\n");
        return 1;
    }

    // Lê o conteúdo do arquivo data.csv
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *csv = malloc(fileSize + 1);
    if (!csv) {
        fclose(file);
        fprintf(stderr, "Erro de alocação de memória\n");
        return 1;
    }

    fread(csv, 1, fileSize, file);
    csv[fileSize] = '\0';
    fclose(file);

    printf("Original CSV:\n%s\n", csv);
    printf("Processed CSV:\n");

    processCsv(csv, selectedColumns, rowFilterDefinitions);

    free(csv);
    return 0;
}
