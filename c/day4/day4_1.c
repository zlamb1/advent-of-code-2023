#include "../dyn_arr.h"
#include "../str.h"
#include "card_list.h"

static const char* file_name = "day4.txt";

card_t parse_card(line_t* line) {
    card_t card = {};
    ASSERT_STR_CMP(line, "Card");
    CONSUME_WHITESPACE(line);
    ASSERT_STR_INT(line, &card.id); 
    ASSERT_STR_CMP(line, ":"); 
    dyn_arr_init(&card.numbers);
    dyn_arr_init(&card.winners); 
    CONSUME_WHITESPACE(line);
    char c = line_get_pos_char(line);
    while (c != '|') {
        int num;
        ASSERT_STR_INT(line, &num);
        dyn_arr_append(&card.numbers, num);
        CONSUME_WHITESPACE(line);
        c = line_get_pos_char(line);
    }
    line->pos++;
    c = line_get_pos_char(line);
    while (c != '\0') {
        CONSUME_WHITESPACE(line);
        int num;
        ASSERT_STR_INT(line, &num);
        dyn_arr_append(&card.winners, num);
        c = line_get_pos_char(line);
    }
    return card; 
}

int parse_cards(file_content_t* file_content) {
    int sum = 0; 
    for (size_t i = 0; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i);
        card_t card = parse_card(line);
        for (size_t i = 0; i < card.numbers.len; i++) {
            int num = *(card.numbers.data + i); 
            if (dyn_arr_find(&card.winners, num) > -1) {
                if (card.score == 0)
                    card.score = 1;
                else card.score *= 2;
            }
        }
        sum += card.score;
    }
    return sum; 
}

int main(void) {
    file_content_t file_content = {};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    int sum = parse_cards(&file_content);
    printf("%i\n", sum);
    content_free(&file_content);
    return EXIT_SUCCESS;
}