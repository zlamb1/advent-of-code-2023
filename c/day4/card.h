#ifndef CARD_LIST_H
#define CARD_LIST_H

#include "../array.h"

MAKE_ARRAY(int)

typedef struct card {
    int id, index, score;
    int_array_t numbers, winners;
} card_t;

MAKE_ARRAY(card, card_t)

void card_free(card_t card) {
    int_array_free(&card.numbers);
    int_array_free(&card.winners);
}

#endif