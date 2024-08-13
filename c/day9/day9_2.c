#include "../array.h"
#include "../str.h"
#include <stdio.h>

MAKE_ARRAY(int)
MAKE_ARRAY(set, int_array_t)

static const char* file_name = "day9.txt";

void __set_destructor(int_array_t set) {
    int_array_free(&set);
}

set_array_t parse_sets(file_content_t* file_content) {
    set_array_t sets;
    set_array_init(&sets); 
    for (size_t i = 0; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i); 
        int_array_t set;
        int_array_init(&set);
        while (line_get_pos_char(line) != '\0') {
            CONSUME_WHITESPACE(line);
            int num; 
            ASSERT_STR_INT(line, &num);
            int_array_append(&set, num); 
        }
        set_array_append(&sets, set);
    }
    return sets;
}

long parse_seq(set_array_t* sets) {
    long counter = 0;
    for (size_t i = 0; i < sets->len; i++) {
        int_array_t set = *(sets->data + i); 
        set_array_t tree;
        set_array_init(&tree);
        tree.destructor_fn = &__set_destructor;
        set_array_append(&tree, set);
        while (tree.len > 0) {
            bool all_zeroes = true;
            int_array_t set = *(tree.data + tree.len - 1);
            int_array_t new_set;
            int_array_init(&new_set);
            for (size_t i = 1; i < set.len; i++) {
                int a = *(set.data + i - 1), b = *(set.data + i);
                int_array_append(&new_set, b - a); 
                if (b - a != 0)
                    all_zeroes = false; 
            }
            if (set.len == 1)
                int_array_append(&new_set, 0);
            set_array_append(&tree, new_set);
            if (all_zeroes)
                break;
        }
        for (size_t i = tree.len; i >= 2; i--) {
            int_array_t* set = tree.data + (i - 2), *p_set = tree.data + (i - 1);
            if (set->len < 1 || p_set->len < 1) {
                fprintf(stderr, "encountered empty set\n");
                continue;
            }
            int new_el = *set->data - *p_set->data;
            int_array_insert(set, new_el, 0); 
            if (i == 2) {
                counter += new_el;
            }
        }
        set_array_free(&tree);
    }
    return counter;
}

int main(void) {
    file_content_t file_content;
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    set_array_t sets = parse_sets(&file_content);
    printf("%ld\n", parse_seq(&sets));
    set_array_free(&sets);
    content_free(&file_content);
    return 0;
}