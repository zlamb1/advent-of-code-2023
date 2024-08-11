#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../str_buf.h"

#include "gear_list.h"
#include "part_list.h"

static const char* file_name = "day3.txt";

static bool is_numeric(char c) {
    int num = c - '0'; 
    return num > -1 && num < 10; 
}

static part_t* parse_part(str_buf_t* buf, int* i, int line_len) {
    int adjacent_offsets[8] = { -line_len - 1, -line_len, -line_len + 1, -1, 1, line_len - 1, line_len, line_len + 1 };
    char c = *(buf->data + *i); 
    // zero initialize struct
    part_t* part = calloc(1, sizeof(part_t));
    int_array_init(&part->adjacent_gears);
    while (is_numeric(c)) {
        part->num = part->num * 10 + (c - '0'); 
        for (size_t j = 0; j < 8; j++) {
            int adjacent_index = *i + adjacent_offsets[j]; 
            if (adjacent_index > -1 && adjacent_index < buf->len) {
                char adjacent_char = *(buf->data + adjacent_index);
                if (adjacent_char == '*') {
                    if (int_array_find(&part->adjacent_gears, adjacent_index) < 0) {
                        int_array_append(&part->adjacent_gears, adjacent_index); 
                        part->adjacent_to_symbol = true;
                    }
                    break;
                }
                if (adjacent_char != '.' && !is_numeric(adjacent_char)) {
                    part->adjacent_to_symbol = true;
                    break;
                }
            }
        }
        (*i)++;
        c = *(buf->data + *i); 
    }
    return part;
}

static part_list_t* parse_parts(str_buf_t* buf) {
    size_t line_len = buf->max_line_len; 
    part_list_t* part_list = calloc(1, sizeof(part_list_t));
    part_list_init(part_list); 
    for (int i = 0; i < buf->len; i++) {
        char c = *(buf->data + i);
        if (is_numeric(c)) {
            part_t* part = parse_part(buf, &i, line_len);
            part_list_append(part_list, part);
        }
    }
    return part_list;
}

int main(void) {
    str_buf_t buf;
    FILE_READ_CODE_T err_code = read_file(file_name, &buf); 
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE; 
    }
    part_list_t* part_list = parse_parts(&buf);
    gear_list_t gear_list;
    gear_list_init(&gear_list); 
    for (size_t i = 0; i < part_list->len; i++) {
        part_t* part = *(part_list->parts + i); 
        for (size_t j = 0; j < part->adjacent_gears.len; j++) {
            int offset = *(part->adjacent_gears.data + j); 
            gear_t* found = gear_list_find(&gear_list, offset);
            if (found == NULL) {
                gear_t* gear = calloc(1, sizeof(gear_t));
                gear->offset = offset;
                gear->adjacent_parts[0] = part->num; 
                gear->len = 1; 
                gear_list_append(&gear_list, gear);
            } else {
                if (found->len < 2)
                    found->adjacent_parts[found->len] = part->num; 
                found->len++;
            }
        }
    }
    int sum = 0;
    for (size_t i = 0; i < gear_list.len; i++) {
        gear_t* gear = *(gear_list.gears + i);
        if (gear->len == 2) {
            gear->ratio = gear->adjacent_parts[0] * gear->adjacent_parts[1];
            sum += gear->ratio;
        }
    }
    printf("%i\n", sum);
    gear_list_free(&gear_list);
    part_list_free(part_list);
    str_buf_free(&buf); 
    return EXIT_SUCCESS;
}