#ifndef STR_BUF_H
#define STR_BUF_H

#include <string.h>

#include "file_read_code.h"
#include "macro.h"

/**
 * str_buf is a struct that holds the raw char data of any file and omits lines
 */

typedef struct str_buf {
    char* data; 
    size_t len, capacity, max_line_len;
} str_buf_t; 

int str_buf_init(str_buf_t* buf) {
    buf->data = malloc(sizeof(char));
    if (buf->data == NULL)
        return -1; 
    buf->len = 0;
    buf->capacity = 1; 
    buf->max_line_len = 0;
    return 0; 
}

int str_buf_alloc(str_buf_t* buf) {
    while (buf->len >= buf->capacity)
        buf->capacity *= 2;
    char* data = malloc(sizeof(char) * buf->capacity);
    if (data == NULL)
        return -1; 
    memcpy(data, buf->data, sizeof(char) * buf->len); 
    free(buf->data);
    buf->data = data; 
    return 0; 
}

int str_buf_append(str_buf_t* buf, char c) {
    int err_code = str_buf_alloc(buf); 
    if (err_code != 0)
        return err_code; 
    *(buf->data + buf->len) = c; 
    buf->len++; 
    return 0;
}

void str_buf_free(str_buf_t* buf) {
    free(buf->data); 
}

FILE_READ_CODE_T read_file(const char* file_name, str_buf_t* buf) {
    ASSERT_PTR_RET_VAL(buf, FILE_READ_INVALID_ARG);
    int err_code = str_buf_init(buf); 
    if (err_code != 0)
        return FILE_READ_OUT_OF_MEMORY;
    FILE* fptr = fopen(file_name, "r"); 
    ASSERT_PTR_RET_VAL(fptr, FILE_READ_NO_FILE); 
    size_t len = 0;
    int c = fgetc(fptr);
    while (c != EOF) {
        if (c == '\n') {
            c = fgetc(fptr); 
            if (buf->max_line_len < len)
                buf->max_line_len = len;
            len = 0;
            continue;
        }
        str_buf_append(buf, c); 
        len++; 
        c = fgetc(fptr); 
    } 
    fclose(fptr);
    return FILE_READ_SUCCESS;
}

#endif