#ifndef FILE_READ_CODE_H
#define FILE_READ_CODE_H

typedef enum FILE_READ_CODE {
    FILE_READ_SUCCESS         =  0,
    FILE_READ_INVALID_ARG     = -1,
    FILE_READ_NO_FILE         = -2,
    FILE_READ_OUT_OF_MEMORY   = -3
} FILE_READ_CODE_T;

void print_file_read_code(FILE_READ_CODE_T err_code, const char* file_name) {
    switch (err_code) {
        case FILE_READ_INVALID_ARG:
            fprintf(stderr, "error: invalid arg pointer\n");
            break;
        case FILE_READ_NO_FILE:
            fprintf(stderr, "error: could not open file '%s'\n", file_name);
            break;
        case FILE_READ_OUT_OF_MEMORY:
            fprintf(stderr, "error: out of memory\n");
            break;
    }
}


#endif