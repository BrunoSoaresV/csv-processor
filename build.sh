#!/bin/bash

# Nome do arquivo de saída
OUTPUT_FILE="libcsv.so"

# Compilar o código C para gerar o arquivo .so
gcc -Wall -fPIC -c libcsv.c -o libcsv.o
gcc -shared -o $OUTPUT_FILE libcsv.o

# Compilar o programa de testes
gcc -Wall -I. test_libcsv.c -L. -lcsv -o test_libcsv -Wl,-rpath=.

# Limpar arquivos temporários
rm -f libcsv.o

echo "Build completed: $OUTPUT_FILE and test_libcsv"
