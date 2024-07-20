# CSV Processor

**csv-processor** is a C library for processing CSV files. The library allows for selecting specific columns and applying custom filters to the CSV data. It is implemented as a shared library (`libcsv.so`) and includes tests to verify the accuracy and functionality of both mandatory and optional features.

## Features

- **CSV Data Processing:** Select specific columns and apply filters to the rows of CSV data.
- **Supported Filters:** Comparison operators such as `=`, `>`, `<`, `!=`, `>=`, `<=`.
- **Column Selection:** Choose which columns to include in the final output.
- **CSV File Processing:** Support for reading CSV files directly from disk.

## Installation

1. **Clone the repository:**

    ```bash
    git clone https://github.com/BrunoSoaresV/csv-processor.git
    ```

2. **Build the library:**

    Navigate to the repository directory and run the build script:

    ```bash
    cd csv-processor
    ./build.sh
    ```

    This will generate the `libcsv.so` shared library.

3. **Compile the tests:**

    Compile the test program with the shared library:

    ```bash
    gcc -o test_libcsv test_libcsv.c -L. -lcsv -Wl,-rpath=.
    ```

4. **Run the tests:**

    Execute the test binary to ensure everything is working correctly:

    ```bash
    ./test_libcsv
    ```

## Usage

### Function Declarations

- **`void processCsv(const char csv[], const char selectedColumns[], const char rowFilterDefinitions[]);`**

  Processes CSV data provided as a string by applying filters and selecting columns.

- **`void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char rowFilterDefinitions[]);`**

  Processes a CSV file by applying filters and selecting columns.

### Example

```c
const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
processCsv(csv, "header1,header3", "header1>1\nheader3<8");

// Output:
// header1,header3
// 4,6

const char csv_file[] = "path/to/csv_file.csv";
processCsvFile(csv_file, "header1,header3", "header1>1\nheader3<8");

// Output:
// header1,header3
// 4,6
