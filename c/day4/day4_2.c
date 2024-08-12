#include "../str.h"
#include "card.h"

static const char* file_name = "day4.txt";

card_t parse_card(line_t* line) {
    card_t card = {0};
    ASSERT_STR_CMP(line, "Card");
    CONSUME_WHITESPACE(line);
    ASSERT_STR_INT(line, &card.id); 
    ASSERT_STR_CMP(line, ":"); 
    int_array_init(&card.numbers);
    int_array_init(&card.winners); 
    CONSUME_WHITESPACE(line);
    char c = line_get_pos_char(line);
    while (c != '|') {
        int num;
        ASSERT_STR_INT(line, &num);
        int_array_append(&card.numbers, num);
        CONSUME_WHITESPACE(line);
        c = line_get_pos_char(line);
    }
    line->pos++;
    c = line_get_pos_char(line);
    while (c != '\0') {
        CONSUME_WHITESPACE(line);
        int num;
        ASSERT_STR_INT(line, &num);
        int_array_append(&card.winners, num);
        c = line_get_pos_char(line);
    }
    return card; 
}

int calculate_count(card_array_t* card_array, card_t* card, int* copies) {
    int copy = *(copies + card->index);
    if (copy > -1)
        return copy;
    int win_count = 0, card_count = 1;
    for (size_t i = 0; i < card->numbers.len; i++) {
        int num = *(card->numbers.data + i);
        if (int_array_find(&card->winners, num) > -1) {
            win_count++;
        }
    }
    while (win_count > 0) {
        card_t* winner_card = card_array->data + card->index + win_count;
        card_count += calculate_count(card_array, winner_card, copies);
        win_count--;
    }
    *(copies + card->index) = card_count;
    return card_count;
}

int parse_cards(file_content_t* file_content) {
    card_array_t card_array; 
    card_array_init(&card_array); 
    card_array.destructor_fn = &card_free;
    for (size_t i = 0; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i);
        card_t card = parse_card(line);
        card.index = i; 
        card_array_append(&card_array, card);
    }
    int* copies = malloc(sizeof(int) * card_array.len), count = 0; 
    memset(copies, -1, sizeof(int) * card_array.len);
    for (size_t i = card_array.len - 1; i < card_array.len; i--) {
        card_t* card = card_array.data + i; 
        count += calculate_count(&card_array, card, copies);
    }
    card_array_free(&card_array);
    return count; 
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
    return EXIT_SUCCESS;
}