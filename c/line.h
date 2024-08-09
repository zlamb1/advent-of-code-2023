#ifndef LINE_H
#define LINE_H

#include <stdio.h>

#include "macro.h"

#define ASSERT_STR_CMP(line, str) do {\
    STR_CMP_T result = line_cmp_str((line), (str));\
    if (result != STR_CMP_EQUAL) {\
        fprintf(stderr, "line %li: expected '%s'\n", line->line_num, (str));\
        exit(EXIT_FAILURE);\
    }\
} while (0)

#define ASSERT_STR_INT(line, i) do {\
    STR_PARSE_T result = line_parse_int((line), (i));\
    if (result != STR_PARSE_SUCCESS) {\
        fprintf(stderr, "line %li: expected int: '%c'\n", line->line_num, line_get_pos_char(line));\
        exit(EXIT_FAILURE);\
    }\
} while (0)

#define ASSERT_STR_LONG(line, i) do {\
    STR_PARSE_T result = line_parse_long((line), (i));\
    if (result != STR_PARSE_SUCCESS) {\
        fprintf(stderr, "line %li: expected long: '%c'\n", line->line_num, line_get_pos_char(line));\
        exit(EXIT_FAILURE);\
    }\
} while (0)

#define CONSUME_WHITESPACE(line) line_consume_char_seq((line), ' ')

typedef struct line {
    char* str;
    size_t line_num, len, pos, capacity; 
} line_t; 

line_t* init_line() {
    line_t* line = malloc(sizeof(line_t)); 
    ASSERT_PTR(line);
    line->str = malloc(sizeof(char));
    if (line->str == NULL) {
        free(line); 
        return NULL;
    }
    line->line_num = 0; 
    line->len      = 0; 
    line->pos      = 0; 
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

typedef enum STR_CMP {
    STR_CMP_EQUAL         =  0,
    STR_CMP_NEQUAL        = -1,
    STR_CMP_OUT_OF_BOUNDS = -2,
} STR_CMP_T;

STR_CMP_T line_cmp_str(line_t* line, const char* str) {
    size_t len = strlen(str), pos = line->pos;
    if (pos + len >= line->len) 
        return STR_CMP_OUT_OF_BOUNDS;
    line->pos += len; 
    if (strncmp(str, line->str + pos, len) == 0) {
        return STR_CMP_EQUAL; 
    } else return STR_CMP_NEQUAL;
}

STR_CMP_T line_const_cmp_str(line_t* line, const char* str) {
    size_t len = strlen(str), pos = line->pos;
    if (pos + len >= line->len) 
        return STR_CMP_OUT_OF_BOUNDS;
    if (strncmp(str, line->str + pos, len) == 0) {
        return STR_CMP_EQUAL; 
    } else return STR_CMP_NEQUAL;
}

// matches any of the strings in the array to the string after line->pos
// if it matches string it advances line->pos by the string's length
// returns matched string's index or -1 if none are matched
int line_cmp_str_arr(line_t* line, const char* arr[], size_t arr_len) {
    for (size_t i = 0; i < arr_len; i++) {
        if (line_const_cmp_str(line, arr[i]) == STR_CMP_EQUAL) {
            line->pos += strlen(arr[i]); 
            return i; 
        }
    }
    return -1;
}

int line_const_cmp_str_arr(line_t* line, const char* arr[], size_t arr_len) {
    for (size_t i = 0; i < arr_len; i++) {
        if (line_const_cmp_str(line, arr[i]) == STR_CMP_EQUAL)
            return i; 
    }
    return -1;
}

typedef enum STR_PARSE {
    STR_PARSE_SUCCESS      =  0,
    STR_PARSE_INVALID_CHAR = -1
} STR_PARSE_T;

bool line_is_pos_char_numeric(line_t* line) {
    int num = *(line->str + line->pos) - '0';
    return num > -1 && num < 10; 
}

int line_get_pos_char_as_int(line_t* line) {
    return *(line->str + line->pos) - '0';
}

STR_PARSE_T line_parse_int(line_t* line, int* out) {
    int accum = 0;
    bool is_numeric = line_is_pos_char_numeric(line);
    if (!is_numeric)
        return STR_PARSE_INVALID_CHAR;
    while (is_numeric) {
        accum = accum * 10 + line_get_pos_char_as_int(line); 
        line->pos++;
        is_numeric = line_is_pos_char_numeric(line);
    }
    *out = accum; 
    return STR_PARSE_SUCCESS; 
}

STR_PARSE_T line_parse_long(line_t* line, long* out) {
    long accum = 0;
    bool is_numeric = line_is_pos_char_numeric(line);
    if (!is_numeric)
        return STR_PARSE_INVALID_CHAR;
    while (is_numeric) {
        accum = accum * 10L + (long)line_get_pos_char_as_int(line); 
        line->pos++;
        is_numeric = line_is_pos_char_numeric(line);
    }
    *out = accum; 
    return STR_PARSE_SUCCESS; 
}

char line_get_pos_char(line_t* line) {
    return *(line->str + line->pos);
}

void line_consume_char_seq(line_t* line, char c) {
    while (line->pos < line->len && *(line->str + line->pos) == c)
        line->pos++;
}

bool line_is_newline(line_t* line) {
    char c = line_get_pos_char(line);
    return line->len == 1 && (c == '\n' || c == '\0');
}

void line_free(line_t* line) {
    free(line->str); 
    free(line);
}

#endif