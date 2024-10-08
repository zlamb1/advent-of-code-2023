#include "../str.h"
#include "node.h"

MAKE_ARRAY(dir, bool)

static const char* file_name = "day8.txt";

dir_array_t parse_directions(file_content_t* file_content) {
    dir_array_t dir_array;
    dir_array_init(&dir_array); 
    if (file_content->num_lines > 0) {
        line_t* line = *file_content->lines;
        char c = line_get_pos_char(line);
        while (c != '\0') {
            dir_array_append(&dir_array, c == 'R'); 
            line->pos++;
            c = line_get_pos_char(line);
        }
    }
    return dir_array; 
}

node_map_t parse_nodes(file_content_t* file_content) {
    node_map_t node_map;
    node_map_init(&node_map);
    for (size_t i = 1; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i); 
        char c = line_get_pos_char(line);
        if (c != '\0') {
            node_t node = {0}; 
            char_array_init(&node.name); 
            while (c != ' ' && c != '\0') {
                char_array_append(&node.name, c);
                line->pos++; 
                c = line_get_pos_char(line);
            }
            char_array_append(&node.name, '\0'); 
            CONSUME_WHITESPACE(line);
            ASSERT_STR_CMP(line, "=");
            CONSUME_WHITESPACE(line);
            ASSERT_STR_CMP(line, "(");
            char_array_init(&node.paths[0]);
            char_array_init(&node.paths[1]); 
            c = line_get_pos_char(line);
            while (c != ',' && c != '\0') {
                char_array_append(&node.paths[0], c);
                line->pos++;
                c = line_get_pos_char(line);
            }
            char_array_append(&node.paths[0], '\0'); 
            ASSERT_STR_CMP(line, ",");
            CONSUME_WHITESPACE(line);
            c = line_get_pos_char(line);
            while (c != ')' && c != '\0') {
                char_array_append(&node.paths[1], c);
                line->pos++;
                c = line_get_pos_char(line);
            }
            char_array_append(&node.paths[1], '\0'); 
            node_map_insert(&node_map, node.name.data, node); 
        }
    }
    return node_map; 
}

size_t traverse_nodes(dir_array_t* dir_array, node_map_t* node_map) {
    node_t node;
    node_map_lookup(node_map, "AAA", &node); 
    size_t counter = 0;
    while (strcmp(node.name.data, "ZZZ") != 0) {
        size_t instruction = counter % dir_array->len; 
        if (dir_array->data[instruction]) {
            node_map_lookup(node_map, node.paths[1].data, &node); 
        } else {
            node_map_lookup(node_map, node.paths[0].data, &node);
        }
        counter++;
    }
    return counter;
}

int main(void) {
    file_content_t file_content = {0};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    dir_array_t dir_array = parse_directions(&file_content); 
    node_map_t node_map = parse_nodes(&file_content);
    size_t count = traverse_nodes(&dir_array, &node_map);
    printf("%ld\n", count); 
    return 0;
}