#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../str_buf.h"

static const char* file_name = "day3.txt";

typedef struct part {
    int num; 
} part_t; 

static bool is_numeric(char c) {
    int num = c - '0'; 
    return num > -1 && num < 10; 
}

static int parse_parts(str_buf_t* buf) {
    size_t line_len = buf->max_line_len; 
    int sum = 0, adjacent_offsets[8] = { -line_len - 1, -line_len, -line_len + 1, -1, 1, line_len - 1, line_len, line_len + 1 };
    for (int i = 0; i < buf->len; i++) {
        char c = *(buf->data + i);
        if (is_numeric(c)) {
            part_t part = {.num = 0}; 
            bool adjacent_to_symbol = false; 
            while (is_numeric(c)) {
                part.num = part.num * 10 + (c - '0'); 
                if (!adjacent_to_symbol) {
                    for (size_t j = 0; j < 8; j++) {
                        int adjacent_index = i + adjacent_offsets[j]; 
                        if (adjacent_index > -1 && adjacent_index < buf->len) {
                            char adjacent_char = *(buf->data + adjacent_index);
                            //printf("j[%li]: %c\n", j, adjacent_char);
                            if (adjacent_char != '.' && !is_numeric(adjacent_char)) {
                                adjacent_to_symbol = true;
                                break;
                            }
                        }
                    }
                }
                i++;
                c = *(buf->data + i); 
            }
            if (adjacent_to_symbol) {
                printf("identified part: %i\n", part.num);
                sum += part.num;
            }
        }
    }
    return sum; 
}

int main(void) {
    str_buf_t buf;
    FILE_READ_CODE_T err_code = read_file(file_name, &buf); 
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE; 
    }
    printf("part number sum: %i\n", parse_parts(&buf));
    str_buf_free(&buf); 
    return EXIT_SUCCESS;
}