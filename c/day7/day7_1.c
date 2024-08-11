#include "../str.h"

int get_card_value(char c) {
    switch (c) {
        case '2':
            return 1;
        case '3':
            return 2;
        case '4':
            return 3;
        case '5':
            return 4;
        case '6':
            return 5;
        case '7':
            return 6;
        case '8':
            return 7;
        case '9':
            return 8;
        case 'T':
            return 9;
        case 'J':
            return 10;
        case 'Q':
            return 11;
        case 'K':
            return 12;
        case 'A':
            return 13; 
        default:
            return -1; 
    }
}

#include "hand.h"

static const char* file_name = "day7.txt";

hand_type_t get_hand_type(hand_t hand) {
    int card_occurrences[13], max_occurrences = INT_MIN; 
    memset(card_occurrences, 0, sizeof(int) * 13); 
    for (size_t i = 0; i < 5; i++) {
        int card_index = get_card_value(hand.cards[i]) - 1; 
        card_occurrences[card_index]++;
        if (card_occurrences[card_index] > max_occurrences) 
            max_occurrences = card_occurrences[card_index];
    }
    switch (max_occurrences) {
        case 5:
            return FIVE_OF_A_KIND;
        case 4:
            return FOUR_OF_A_KIND;
        case 3:
            for (size_t i = 0; i < 13; i++) {
                if (card_occurrences[i] == 2)
                    return FULL_HOUSE;
            }
            return THREE_OF_A_KIND;
        case 2: {
            int counter = 0;
            for (size_t i = 0; i < 13; i++) {
                if (card_occurrences[i] == 2)
                    counter++;
            }
            if (counter == 2)
                return TWO_PAIR;
            else return ONE_PAIR;
        }
        default:
            return HIGH_CARD;
    }
}

int parse_cards(file_content_t* file_content) {
    hand_array_t hand_array;
    hand_array_init(&hand_array);
    for (size_t i = 0; i < file_content->num_lines; i++) {
        line_t* line = *(file_content->lines + i);
        hand_t hand = {0};
        for (size_t i = 0; i < 5; i++) {
            hand.cards[i] = line_get_pos_char(line);
            line->pos++;
        } 
        CONSUME_WHITESPACE(line);
        ASSERT_STR_LONG(line, &hand.bid);
        hand.type = get_hand_type(hand);
        hand_array_insert(&hand_array, hand); 
    }
    int total = 0;
    for (size_t i = 0; i < hand_array.len; i++) {
        hand_t hand = *(hand_array.data + i); 
        total += hand.bid * (i + 1);
    }
    hand_array_free(&hand_array);
    return total;
}

int main(void) {
    file_content_t file_content = {0};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    printf("%i\n", parse_cards(&file_content)); 
    return 0;
}