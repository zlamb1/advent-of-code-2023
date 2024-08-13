#ifndef HAND_H
#define HAND_H

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../array.h"

typedef enum hand_type {
    HIGH_CARD       = 0,
    ONE_PAIR        = 1,
    TWO_PAIR        = 2,
    THREE_OF_A_KIND = 4,
    FULL_HOUSE      = 5,
    FOUR_OF_A_KIND  = 6,
    FIVE_OF_A_KIND  = 7
} hand_type_t; 

typedef struct hand {
    char cards[5]; 
    hand_type_t type; 
    long bid; 
} hand_t;

MAKE_ARRAY(hand, hand_t)

int get_card_value(char c); 

/**
 * returns true if the value of hand a is greater than b and false otherwise
 */
int compare_hands(hand_t a, hand_t b) {
    if (a.type > b.type)
        return 1; 
    else if (b.type > a.type)
        return -1; 
    for (size_t i = 0; i < 5; i++) {
        int a_card_value = get_card_value(a.cards[i]), b_card_value = get_card_value(b.cards[i]);
        if (a_card_value > b_card_value)
            return 1;
        if (b_card_value > a_card_value)
            return -1;
    }
    return 0;
}

int hand_array_binary_search_reverse(hand_array_t* array, hand_t hand, bool reverse) {
    int low = 0, high = array->len - 1, low_cmp = (reverse ? 1 : -1), high_cmp = (reverse ? -1 : 1);
    while (low <= high) {
        int mid = (low + high) / 2;
        hand_t search_hand = *(array->data + mid);
        int search_val = compare_hands(hand, search_hand);
        if (search_val == low_cmp) {
            low = mid + 1;
        } else if (search_val == high_cmp) {
            high = mid - 1;
        } else {
            return mid;
        }
    }
    return -(low + 1);
}

int hand_array_binary_search(hand_array_t* array, hand_t hand) {
    return hand_array_binary_search_reverse(array, hand, false);
}

/**
 * inserts hand into sorted array based on value using binary search
 */
void hand_array_sorted_insert(hand_array_t* array, hand_t hand) {
    hand_array_alloc(array); 
    if (array->len == 0) {
        *array->data = hand; 
        array->len++;
    } else {
        int index = hand_array_binary_search_reverse(array, hand, true);
        if (index < 0) index = -index - 1; 
        if (index != array->len) {
            memmove(array->data + index + 1, array->data + index, sizeof(hand_t) * (array->len - index));
        } 
        *(array->data + index) = hand;
        array->len++;
    }
}

void hand_list_free(hand_array_t* array) {
    free(array->data);
}

#endif