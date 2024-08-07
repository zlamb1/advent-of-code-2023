#ifndef STR_H
#define STR_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_read_code.h"
#include "line.h"

typedef struct file_content {
    line_t** lines; 
    size_t num_lines, capacity;
} file_content_t; 

int content_init(file_content_t* file_content) {
    file_content->lines = malloc(sizeof(line_t*));
    if (file_content->lines == NULL) {
        free(file_content); 
        return -1;
    }
    file_content->num_lines = 0;
    file_content->capacity = 1;
    return 0;
}

int content_alloc(file_content_t* file_content) {
    bool alloc = false;
    while (file_content->num_lines >= file_content->capacity) {
        file_content->capacity *= 2; 
        alloc = true; 
    }
    if (!alloc)
        return 0; 
    line_t** arr = malloc(sizeof(line_t*) * file_content->capacity);
    ASSERT_PTR_RET_VAL(arr, -1);
    memcpy(arr, file_content->lines, sizeof(line_t*) * file_content->num_lines);
    free(file_content->lines);
    file_content->lines = arr; 
    return 0; 
}

int content_append(file_content_t* file_content, line_t* line) {
    int err_code = content_alloc(file_content);
    if (err_code != 0)
        return err_code;
    line->line_num = file_content->num_lines + 1;
    *(file_content->lines + file_content->num_lines) = line;
    file_content->num_lines++;
    return 0;
}

void content_free(file_content_t* file_content) {
    for (size_t i = 0; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i);
        line_free(line);
    }
    free(file_content->lines);
}

line_t* read_line(FILE* fptr) {
    line_t* line = init_line(); 
    int c = fgetc(fptr); 
    if (c == EOF) return NULL; 
    while (c != EOF && c != '\n') {
        int err_code = line_append(line, c);
        if (err_code != 0)
            return NULL; 
        c = fgetc(fptr); 
    }
    int err_code = line_append(line, '\0'); 
    if (err_code != 0)
        return NULL; 
    return line;
}

FILE_READ_CODE_T read_file(file_content_t* file_content, const char* file_name) {
    ASSERT_PTR_RET_VAL(file_content, FILE_READ_INVALID_ARG);
    FILE* fptr = fopen(file_name, "r"); 
    ASSERT_PTR_RET_VAL(fptr, FILE_READ_NO_FILE); 
    int err_code = content_init(file_content);
    line_t* line;
    while ((line = read_line(fptr)) != NULL) {
        ASSERT_PTR_RET_VAL(line, FILE_READ_OUT_OF_MEMORY);
        int err_code = content_append(file_content, line);
        if (err_code != 0)
            return FILE_READ_OUT_OF_MEMORY;
    }
    fclose(fptr);
    return FILE_READ_SUCCESS;
}

#endif