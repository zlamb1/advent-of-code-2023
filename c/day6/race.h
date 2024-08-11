#ifndef RACE_H
#define RACE_H

#include <stdlib.h>
#include <string.h>

typedef struct race {
    long time, distance;
} race_t;

typedef struct race_list {
    race_t* races; 
    size_t len, capacity;
} race_list_t;

void race_list_init(race_list_t* race_list) {
    race_list->races = malloc(sizeof(race_t)); 
    race_list->len = 0;
    race_list->capacity = 1; 
}

void race_list_alloc(race_list_t* race_list) {
    while (race_list->len >= race_list->capacity) 
        race_list->capacity *= 2;
    race_t* races = malloc(sizeof(race_t) * race_list->capacity);
    memcpy(races, race_list->races, sizeof(race_t) * race_list->len);
    free(race_list->races);
    race_list->races = races;
}

void race_list_append(race_list_t* race_list, race_t race) {
    race_list_alloc(race_list);
    *(race_list->races + race_list->len) = race;
    race_list->len++;
}

void race_list_free(race_list_t* race_list) {
    free(race_list->races);
}

#endif