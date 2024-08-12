#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../str_buf.h"

#include "gear.h"
#include "part.h"

static const char* file_name = "day3.txt";

static bool is_numeric(char c) {
    int num = c - '0'; 
    return num > -1 && num < 10; 
}

static part_t parse_part(str_buf_t* buf, int* i, int line_len) {
    int adjacent_offsets[8] = { -line_len - 1, -line_len, -line_len + 1, -1, 1, line_len - 1, line_len, line_len + 1 };
    char c = *(buf->data + *i); 
    // zero initialize struct
    part_t part = {0}; 
    int_array_init(&part.adjacent_gears);
    while (is_numeric(c)) {
        part.num = part.num * 10 + (c - '0'); 
        for (size_t j = 0; j < 8; j++) {
            int adjacent_index = *i + adjacent_offsets[j]; 
            if (adjacent_index > -1 && adjacent_index < buf->len) {
                char adjacent_char = *(buf->data + adjacent_index);
                if (adjacent_char == '*') {
                    if (int_array_find(&part.adjacent_gears, adjacent_index) < 0) {
                        int_array_append(&part.adjacent_gears, adjacent_index); 
                        part.adjacent_to_symbol = true;
                    }
                    break;
                }
                if (adjacent_char != '.' && !is_numeric(adjacent_char)) {
                    part.adjacent_to_symbol = true;
                    break;
                }
            }
        }
        (*i)++;
        c = *(buf->data + *i); 
    }
    return part;
}

static part_array_t* parse_parts(str_buf_t* buf) {
    size_t line_len = buf->max_line_len; 
    part_array_t* part_array = calloc(1, sizeof(part_array_t));
    part_array_init(part_array); 
    for (int i = 0; i < buf->len; i++) {
        char c = *(buf->data + i);
        if (is_numeric(c)) {
            part_array_append(part_array, parse_part(buf, &i, line_len));
        }
    }
    return part_array;
}

int main(void) {
    str_buf_t buf;
    FILE_READ_CODE_T err_code = read_file(file_name, &buf); 
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE; 
    }
    part_array_t* part_array = parse_parts(&buf);
    gear_array_t gear_array;
    gear_array_init(&gear_array); 
    for (size_t i = 0; i < part_array->len; i++) {
        part_t part = *(part_array->data + i); 
        for (size_t j = 0; j < part.adjacent_gears.len; j++) {
            int offset = *(part.adjacent_gears.data + j); 
            gear_t* found = gear_array_find_by_offset(&gear_array, offset);
            if (found == NULL) {
                gear_t gear = {0};
                gear.offset = offset;
                gear.adjacent_parts[0] = part.num; 
                gear.len = 1; 
                gear_array_append(&gear_array, gear);
            } else {
                if (found->len < 2)
                    found->adjacent_parts[found->len] = part.num; 
                found->len++;
            }
        }
    }
    int sum = 0;
    for (size_t i = 0; i < gear_array.len; i++) {
        gear_t* gear = gear_array.data + i;
        if (gear->len == 2) {
            gear->ratio = gear->adjacent_parts[0] * gear->adjacent_parts[1];
            sum += gear->ratio;
        }
    }
    printf("%i\n", sum);
    return EXIT_SUCCESS;
}