#ifndef STR_H
#define STR_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_read_code.h"
#include "line.h"
#include "macro.h"

typedef struct file_content {
    line_t** lines; 
    size_t num_lines, capacity;
} file_content_t; 

int content_init(file_content_t* file_content) {
    file_content->lines = malloc(sizeof(line_t*));
    file_content->num_lines = 0;
    file_content->capacity = 0;
    if (file_content->lines == NULL) {
        free(file_content); 
        return -1;
    }
    file_content->capacity = 1; 
    return 0;
}

int content_alloc(file_content_t* file_content) {
    size_t old_capacity = file_content->capacity;
    while (file_content->num_lines >= file_content->capacity)
        file_content->capacity = ceil(file_content->capacity * 1.5); 
    if (file_content->capacity == old_capacity) return 0; 
    line_t** new_lines = malloc(sizeof(line_t*) * file_content->capacity);
    if (new_lines == NULL) {
        file_content->capacity = old_capacity;
        return -1; 
    }
    memcpy(new_lines, file_content->lines, sizeof(line_t*) * file_content->num_lines);
    free(file_content->lines);
    file_content->lines = new_lines; 
    return 0; 
}

int content_append(file_content_t* file_content, line_t* line) {
    if (content_alloc(file_content) != 0)
        return -1;
    *(file_content->lines + file_content->num_lines) = line;
    file_content->num_lines++;
    line->line_num = file_content->num_lines;
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
    int c = fgetc(fptr); 
    if (c == EOF) return NULL; 
    line_t* line = init_line();
    if (line == NULL) return NULL;
    while (c != EOF && c != '\n') {
        if (line_append(line, c) != 0) {
            line_free(line);
            return NULL; 
        }
        c = fgetc(fptr); 
    }
    if (line_append(line, '\0') != 0) {
        line_free(line);
        return NULL; 
    }
    return line;
}

FILE_READ_CODE_T read_file(file_content_t* file_content, const char* file_name) {
    ASSERT_PTR_RET_VAL(file_content, FILE_READ_INVALID_ARG);
    FILE* fptr = fopen(file_name, "r"); 
    if (fptr == NULL) return FILE_READ_NO_FILE;
    content_init(file_content);
    line_t* line;
    while ((line = read_line(fptr)) != NULL) {
        if (content_append(file_content, line) != 0) {
            fclose(fptr);
            return FILE_READ_OUT_OF_MEMORY;
        }
    }
    fclose(fptr);
    return FILE_READ_SUCCESS;
}

#endif