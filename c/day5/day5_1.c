#include <limits.h>

#include "../l_dyn_arr.h"
#include "../str.h"
#include "almanac.h"

static const char* file_name = "day5.txt";

num_map_t* parse_map(file_content_t* file_content, size_t* index) {  
    num_map_t* map = calloc(1, sizeof(num_map_t));
    num_map_init(map);
    line_t* line = *(file_content->lines + *index);
    while (line_is_pos_char_numeric(line)) {
        range_t range = {};
        ASSERT_STR_LONG(line, &range.dst);
        CONSUME_WHITESPACE(line);
        ASSERT_STR_LONG(line, &range.src);
        CONSUME_WHITESPACE(line);
        ASSERT_STR_LONG(line, &range.len);
        CONSUME_WHITESPACE(line);
        num_map_append(map, range);
        (*index)++;
        if (*index < file_content->num_lines)
            line = *(file_content->lines + *index);
        else break;
    }
    return map; 
}

long process_seeds(dyn_arr_t* seeds, almanac_t* almanac) {
    long min = LONG_MAX;
    for (size_t i = 0; i < seeds->len; i++) {
        long num = *(seeds->data + i); 
        for (size_t j = 0; j < almanac->len; j++) {
            num_map_t* map = *(almanac->maps + j); 
            for (size_t k = 0; k < map->len; k++) {
                range_t range = *(map->ranges + k); 
                int mapping = num - range.src; 
                if (mapping > -1 && mapping < range.len) {
                    num = range.dst + mapping;
                    break;
                }
            }
        }
        if (num < min)
            min = num;
    }
    return min;
}

long parse_maps(file_content_t* file_content) {
    dyn_arr_t seeds;
    dyn_arr_init(&seeds);
    if (file_content->num_lines > 0) {
        line_t* line = *file_content->lines;
        ASSERT_STR_CMP(line, "seeds:");
        char c = line_get_pos_char(line);
        while (c != '\0') {
            CONSUME_WHITESPACE(line);
            long seed;
            ASSERT_STR_LONG(line, &seed);
            dyn_arr_append(&seeds, seed);
            c = line_get_pos_char(line);
        }
    }
    almanac_t almanac = {};
    almanac_init(&almanac);
    size_t len = 0; 
    for (size_t i = 1; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i); 
        if (line_is_newline(line))
            continue;
        if (line_is_pos_char_numeric(line)) {
            num_map_t* map = parse_map(file_content, &i);
            almanac_append(&almanac, map);
        }
    }
    long min = process_seeds(&seeds, &almanac);
    almanac_free(&almanac);
    dyn_arr_free(&seeds);
    return min;
}

int main(void) {
    file_content_t file_content = {};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    long min = parse_maps(&file_content);
    printf("%ld\n", min);
    content_free(&file_content);
    return EXIT_SUCCESS;
}