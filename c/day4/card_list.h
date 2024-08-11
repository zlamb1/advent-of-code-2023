#ifndef CARD_LIST_H
#define CARD_LIST_H

#include "../array.h"

MAKE_ARRAY(int, int)

typedef struct card {
    int id, index, score;
    int_array_t numbers, winners;
} card_t;

typedef struct card_list {
    card_t** cards;
    size_t len, capacity; 
} card_list_t;

void card_list_init(card_list_t* list) {
    list->cards = malloc(sizeof(card_t*)); 
    list->len = 0;
    list->capacity = 1; 
}

void card_list_alloc(card_list_t* list) {
    while (list->len >= list->capacity)
        list->capacity *= 2;
    card_t** cards = malloc(sizeof(card_t*) * list->capacity);
    memcpy(cards, list->cards, sizeof(card_t*) * list->len); 
    free(list->cards);
    list->cards = cards;
}

void card_list_append(card_list_t* list, card_t* card) {
    card_list_alloc(list); 
    *(list->cards + list->len) = card;
    list->len++; 
}

void card_free(card_t* card) {
    int_array_free(&card->numbers);
    int_array_free(&card->winners);
    free(card);
}

void card_list_free(card_list_t* list) {
    for (size_t i = 0; i < list->len; i++) {
        card_t* card = *(list->cards + i);
        card_free(card);
    }
    free(list->cards); 
}

#endif