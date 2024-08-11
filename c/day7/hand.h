#ifndef HAND_H
#define HAND_H

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct hand_list {
    hand_t* hands;
    size_t len, capacity;
} hand_list_t;

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

void hand_list_init(hand_list_t* hand_list) {
    hand_list->hands = malloc(sizeof(hand_t));
    hand_list->len = 0;
    hand_list->capacity = 1;
}

void hand_list_alloc(hand_list_t* hand_list) {
    while (hand_list->len >= hand_list->capacity)
        hand_list->capacity *= 2;
    hand_t* hands = malloc(sizeof(hand_t) * hand_list->capacity);
    memcpy(hands, hand_list->hands, sizeof(hand_t) * hand_list->len); 
    free(hand_list->hands);
    hand_list->hands = hands;
}

int hand_list_binary_search_reverse(hand_list_t* hand_list, hand_t hand, bool reverse) {
    int low = 0, high = hand_list->len - 1, low_cmp = (reverse ? 1 : -1), high_cmp = (reverse ? -1 : 1);
    while (low <= high) {
        int mid = (low + high) / 2;
        hand_t search_hand = *(hand_list->hands + mid);
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

int hand_list_binary_search(hand_list_t* hand_list, hand_t hand) {
    return hand_list_binary_search_reverse(hand_list, hand, false);
}

/**
 * inserts hand into sorted array based on value using binary search
 */
void hand_list_insert(hand_list_t* hand_list, hand_t hand) {
    hand_list_alloc(hand_list); 
    if (hand_list->len == 0) {
        *hand_list->hands = hand; 
        hand_list->len++;
    } else {
        int index = hand_list_binary_search_reverse(hand_list, hand, true);
        if (index < 0) index = -index - 1; 
        if (index != hand_list->len) {
            memmove(hand_list->hands + index + 1, hand_list->hands + index, sizeof(hand_t) * (hand_list->len - index));
        } 
        *(hand_list->hands + index) = hand;
        hand_list->len++;
    }
}

void hand_list_free(hand_list_t* hand_list) {
    free(hand_list->hands);
}

#endif