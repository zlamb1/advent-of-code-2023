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

long process_seeds(seed_list_t* seed_list, almanac_t* almanac) {
    long min = LONG_MAX;
    while (seed_list->len > 0) {
        seed_range_t seed_range = *(seed_list->ranges + seed_list->len - 1); 
        seed_list->len--;
        if (seed_range.map >= almanac->len) {
            if (min > seed_range.start)
                min = seed_range.start;
            continue;
        }
        num_map_t* num_map = *(almanac->maps + seed_range.map); 
        if (seed_range.range >= num_map->len) {
            if (!seed_range.matched) {
                seed_range.map += 1;
                seed_range.range = 0;
                seed_range.matched = false;
                seed_list_append(seed_list, seed_range);
            }
            continue;
        }
        for (size_t i = seed_range.range; i < num_map->len; i++) {
            range_t num_range = *(num_map->ranges + i);
            long start = num_range.src, end = num_range.src + num_range.len - 1; 
            if (seed_range.start <= start && seed_range.end >= end) {
                // seed_range encapsulates range
                if (seed_range.start < start) {
                    seed_list_append(seed_list, (seed_range_t){
                        .start   = seed_range.start,
                        .end     = start - 1,
                        .map     = seed_range.map,
                        .range   = i + 1,
                        .matched = seed_range.matched
                    });
                }
                seed_list_append(seed_list, (seed_range_t){
                    .start = num_range.dst,
                    .end   = num_range.dst + (end - start),
                    .map   = seed_range.map + 1
                });
                seed_list_append(seed_list, (seed_range_t){
                    .start   = start,
                    .end     = end,
                    .map     = seed_range.map,
                    .range   = i + 1,
                    .matched = true
                });
                if (seed_range.end > end) {
                    seed_list_append(seed_list, (seed_range_t){
                        .start   = end + 1,
                        .end     = seed_range.end,
                        .map     = seed_range.map,
                        .range   = i + 1,
                        .matched = seed_range.matched
                    });
                }
                break;
            } else if (seed_range.start >= start && seed_range.end <= end) {
                // seed_range is encapsulated by range
                seed_range.range = i + 1;
                seed_range.matched = true;
                seed_list_append(seed_list, seed_range);
                seed_list_append(seed_list, (seed_range_t){
                    .start = num_range.dst + (seed_range.start - start),
                    .end   = num_range.dst + (seed_range.start - start) + (seed_range.end - seed_range.start),
                    .map   = seed_range.map + 1
                });
                break;
            } else if (seed_range.start < start && seed_range.end >= start) {
                // seed_range overlaps range from the left
                seed_list_append(seed_list, (seed_range_t){
                    .start   = seed_range.start,
                    .end     = start - 1,
                    .map     = seed_range.map,
                    .range   = i + 1,
                    .matched = seed_range.matched
                });
                seed_list_append(seed_list, (seed_range_t){
                    .start   = start,
                    .end     = start + seed_range.end - start,
                    .map     = seed_range.map,
                    .range   = i + 1,
                    .matched = true
                });
                seed_list_append(seed_list, (seed_range_t){
                    .start = num_range.dst,
                    .end   = num_range.dst + seed_range.end - start,
                    .map   = seed_range.map + 1
                });
                break;
            } else if (seed_range.start <= end && seed_range.end > end) {
                // seed_range overlaps range from the right
                seed_list_append(seed_list, (seed_range_t){
                    .start = num_range.dst + seed_range.start - start,
                    .end   = num_range.dst + end - start,
                    .map   = seed_range.map + 1
                });
                seed_list_append(seed_list, (seed_range_t){
                    .start   = seed_range.start,
                    .end     = end,
                    .map     = seed_range.map,
                    .range   = i + 1,
                    .matched = true
                });
                seed_list_append(seed_list, (seed_range_t){
                    .start   = end + 1,
                    .end     = seed_range.end,
                    .map     = seed_range.map,
                    .range   = i + 1,
                    .matched = seed_range.matched
                });
                break;
            } else if (i == num_map->len - 1 && !seed_range.matched) {
                // no ranges mapped in num_map[i]
                seed_range.map += 1;
                seed_range.range = 0;
                seed_range.matched = false;
                seed_list_append(seed_list, seed_range);
                break;
            }
        }
    }
    return min;
}

long parse_maps(file_content_t* file_content) {
    seed_list_t seed_list = {};
    seed_list_init(&seed_list);
    if (file_content->num_lines > 0) {
        line_t* line = *file_content->lines;
        ASSERT_STR_CMP(line, "seeds:");
        char c = line_get_pos_char(line);
        while (c != '\0') {
            seed_range_t range = {};
            CONSUME_WHITESPACE(line);
            ASSERT_STR_LONG(line, &range.start);
            CONSUME_WHITESPACE(line);
            ASSERT_STR_LONG(line, &range.end);
            range.end = range.start + range.end - 1;
            seed_list_append(&seed_list, range);
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
    long min = process_seeds(&seed_list, &almanac);
    almanac_free(&almanac);
    seed_list_free(&seed_list);
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