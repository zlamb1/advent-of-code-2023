#include "../str.h"

int get_card_value(char c) {
    switch (c) {
        case 'J':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'T':
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
    int card_occurrences[13], max_occurrences = INT_MIN, joker_occurrences = 0; 
    memset(card_occurrences, 0, sizeof(int) * 13); 
    for (size_t i = 0; i < 5; i++) {
        if (hand.cards[i] == 'J')
            joker_occurrences++;
        int card_index = get_card_value(hand.cards[i]) - 1; 
        card_occurrences[card_index]++;
        if (card_occurrences[card_index] > max_occurrences) 
            max_occurrences = card_occurrences[card_index];
    }
    switch (max_occurrences) {
        case 5:
            return FIVE_OF_A_KIND;
        case 4:
            if (joker_occurrences > 0)         // JAAAA -> AAAAA | JJJJA -> AAAAA
                return FIVE_OF_A_KIND;
            return FOUR_OF_A_KIND;
        case 3: {
            bool full_house = false;
            for (size_t i = 0; i < 13; i++) {
                if (card_occurrences[i] == 2) {
                    full_house = true;
                    break;
                }
            }
            switch (joker_occurrences) {
                case 3:
                    if (full_house)            // JJJAA -> AAAAA
                        return FIVE_OF_A_KIND;
                    return FOUR_OF_A_KIND;     // JJJAK -> AAAAK
                case 2:
                    return FIVE_OF_A_KIND;     // JJAAA -> AAAAA
                case 1:
                    return FOUR_OF_A_KIND;     // JAAAK -> AAAAK
            }
            if (full_house)
                return FULL_HOUSE;
            return THREE_OF_A_KIND;
        }
        case 2: {
            int counter = 0;
            for (size_t i = 0; i < 13; i++) {
                if (card_occurrences[i] == 2)
                    counter++;
            }
            switch (joker_occurrences) {
                case 2:
                    if (counter == 2)          // JJAA2 -> AAAA2
                        return FOUR_OF_A_KIND;
                    return THREE_OF_A_KIND;    // JJAQ2 -> AAAQ2
                case 1:
                    if (counter == 2)          // AAQQJ -> AAQQQ
                        return FULL_HOUSE;
                    return THREE_OF_A_KIND;    // AAKQJ -> AAKQA
            }
            if (counter == 2)
                return TWO_PAIR;
            else return ONE_PAIR;
        }
        default:
            if (joker_occurrences == 1)        // JAKQT -> AAKQT
                return ONE_PAIR;
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
        hand_array_sorted_insert(&hand_array, hand); 
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