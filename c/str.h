#ifndef STR_H
#define STR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_PTR(ptr) do {\
    if ((ptr) == NULL) {\
        return NULL;\
    }\
} while (0)

#define ASSERT_PTR_RET_VAL(ptr, ret_val) do {\
    if ((ptr) == NULL) {\
        return (ret_val);\
    }\
} while (0)

typedef struct line {
    char* str;
    size_t len, capacity; 
} line_t; 

typedef struct file_content {
    line_t** lines; 
    size_t num_lines, capacity;
} file_content_t; 

line_t* init_line() {
    line_t* line = malloc(sizeof(line_t)); 
    ASSERT_PTR(line);
    line->str = malloc(sizeof(char));
    if (line->str == NULL) {
        free(line); 
        return NULL;
    }
    line->len = 0; 
    line->capacity = 1; 
    return line;
}

int line_alloc(line_t* line) {
    bool alloc = false;
    while (line->len >= line->capacity) {
        line->capacity *= 2; 
        alloc = true;
    }
    if (!alloc)
        return 0;
    char* sptr = malloc(sizeof(char) * line->capacity); 
    ASSERT_PTR_RET_VAL(sptr, -1);
    memcpy(sptr, line->str, sizeof(char) * line->len); 
    free(line->str);
    line->str = sptr; 
    return 0; 
}

int line_append(line_t* line, char c) {
    int err_code = line_alloc(line);
    if (err_code != 0)
        return err_code;
    *(line->str + line->len) = c;
    line->len++;
    return 0;  
}

void line_free(line_t* line) {
    free(line->str); 
    free(line);
}

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

typedef enum FILE_READ_CODE {
    SUCCESS     =  0,
    INVALID_ARG = -1,
    NO_FILE     = -2,
    NO_MEMORY   = -3
} FILE_READ_CODE_T;

FILE_READ_CODE_T read_file(file_content_t* file_content, const char* file_name) {
    ASSERT_PTR_RET_VAL(file_content, INVALID_ARG);
    FILE* fptr = fopen(file_name, "r"); 
    ASSERT_PTR_RET_VAL(fptr, NO_FILE); 
    int err_code = content_init(file_content);
    line_t* line;
    while ((line = read_line(fptr)) != NULL) {
        ASSERT_PTR_RET_VAL(line, NO_MEMORY);
        int err_code = content_append(file_content, line);
        if (err_code != 0)
            return NO_MEMORY;
    }
    fclose(fptr);
    return SUCCESS;
}

void print_file_read_code(FILE_READ_CODE_T err_code, const char* file_name) {
    switch (err_code) {
        case INVALID_ARG:
            fprintf(stderr, "invalid file content pointer\n");
            break;
        case NO_FILE:
            fprintf(stderr, "could not open file %s\n", file_name);
            break;
        case NO_MEMORY:
            fprintf(stderr, "could not allocate memory\n");
            break;
    }
}

#endif