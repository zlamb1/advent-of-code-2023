#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// general-purpose hash-map implementation that uses a power of two capacity and linear-probing

#define HASH_TABLE_INIT_CAPACITY 16
#define HASH_TABLE_LOAD_FACTOR 0.75

typedef struct entry {
    char* key;
    unsigned long hash;
    void* value;
    bool tombstone; 
} entry_t;

typedef struct hash_map {
    entry_t** entries;
    size_t num_entries, capacity;
} hash_map_t;

typedef enum HASH_MAP_RESULT {
    HASH_MAP_RESULT_SUCCESS       =  0,
    HASH_MAP_RESULT_NULL_ENTRIES  = -1,
    HASH_MAP_RESULT_OUT_OF_MEMORY = -2,
    HASH_MAP_RESULT_NO_CAPACITY   = -3,
    HASH_MAP_RESULT_NOT_FOUND     = -4
} HASH_MAP_RESULT_T;

static unsigned long str_hash(char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (c + 128);
    return hash;
}

static void hash_map_free(hash_map_t*); 

static HASH_MAP_RESULT_T hash_map_init(hash_map_t* hash_map) {
    if (hash_map->entries != NULL) 
        hash_map_free(hash_map);
    hash_map->entries = calloc(HASH_TABLE_INIT_CAPACITY, sizeof(entry_t*));
    if (hash_map->entries == NULL)
        return HASH_MAP_RESULT_OUT_OF_MEMORY;
    hash_map->num_entries = 0; 
    hash_map->capacity = HASH_TABLE_INIT_CAPACITY;
    return HASH_MAP_RESULT_SUCCESS;
}

static HASH_MAP_RESULT_T hash_map_alloc(hash_map_t* hash_map) {
    if (hash_map->entries == NULL)
        return HASH_MAP_RESULT_NULL_ENTRIES;
    size_t old_capacity = hash_map->capacity;
    while ((double)hash_map->num_entries / (double)hash_map->capacity > HASH_TABLE_LOAD_FACTOR)
        hash_map->capacity *= 2; 
    entry_t** entries = calloc(hash_map->capacity, sizeof(entry_t*));
    size_t counter = 0; 
    // move old hashes into new number range
    for (size_t i = 0; i < old_capacity; i++) {
        entry_t* entry = *(hash_map->entries + i);
        if (entry != NULL) {
            if (entry->tombstone) {
                free(entry);
                continue;
            }
            // fast modulo when capacity is a power of 2
            size_t index = entry->hash & (hash_map->capacity - 1), iter = index;
            while (*(entries + iter) != NULL) {
                if (iter == index - 1) {
                    // wrapped-around and found no empty spot?
                    free(entries);
                    return HASH_MAP_RESULT_NO_CAPACITY;
                }
                iter = (iter + 1) & (hash_map->capacity - 1); 
            }
            *(entries + iter) = entry; 
            counter++; 
        }
    }
    free(hash_map->entries);
    hash_map->entries = entries;
    hash_map->num_entries = counter;
    return HASH_MAP_RESULT_SUCCESS;
}

static HASH_MAP_RESULT_T hash_map_insert(hash_map_t* hash_map, char* key, void* value) {
    if (hash_map->entries == NULL)
        return HASH_MAP_RESULT_NULL_ENTRIES;
    // realloc map if size/capacity is below shrink factor or above load factor
    HASH_MAP_RESULT_T result = hash_map_alloc(hash_map); 
    if (result != HASH_MAP_RESULT_SUCCESS)
        return result;
    unsigned long key_hash = str_hash(key);
    size_t index = key_hash & (hash_map->capacity - 1), iter = index; 
    entry_t* p_entry;
    while ((p_entry = *(hash_map->entries + iter)) != NULL) {
        // TODO: refactor strcmp
        if (key_hash == p_entry->hash && strcmp(key, p_entry->key) == 0)
            break;
        if (iter == index - 1)
            // wrapped-around and found no empty spot?
            return HASH_MAP_RESULT_NO_CAPACITY;
        iter = (iter + 1) & (hash_map->capacity - 1); 
    }
    // TODO: alloc entries from a block allocator?
    entry_t* new_entry = calloc(1, sizeof(entry_t));
    new_entry->key = key;
    new_entry->hash = key_hash;
    new_entry->value = value;
    hash_map->num_entries++;
    *(hash_map->entries + iter) = new_entry;
    return HASH_MAP_RESULT_SUCCESS;
}

static void* hash_map_lookup(hash_map_t* hash_map, char* key) {
    if (hash_map->entries == NULL) {
        fprintf(stderr, "attempted lookup on hash_map with null entries\n");
        return NULL;
    }
    unsigned long key_hash = str_hash(key); 
    size_t index = key_hash & (hash_map->capacity - 1), iter = index; 
    entry_t* entry;
    while ((entry = *(hash_map->entries + iter)) != NULL) {
        // TODO: refactor strcmp
        if (!entry->tombstone && key_hash == entry->hash && strcmp(key, entry->key) == 0)
            return entry->value;
        if (iter == index - 1)
            break;
        iter = (iter + 1) & (hash_map->capacity - 1); 
    }
    return NULL;
}

static void* hash_map_delete(hash_map_t* hash_map, char* key) {
    if (hash_map->entries == NULL) {
        fprintf(stderr, "attempted delete on hash_map with null entries\n");
        return NULL;
    }
    unsigned long key_hash = str_hash(key); 
    size_t index = key_hash & (hash_map->capacity - 1), iter = index; 
    entry_t* entry;
    while ((entry = *(hash_map->entries + iter)) != NULL) {
        // TODO: refactor strcmp
        if (!entry->tombstone && key_hash == entry->hash && strcmp(key, entry->key) == 0) {
            hash_map->num_entries--;
            entry->tombstone = true; 
            return entry->value;
        }
        if (iter == index - 1)
            break;
        iter = (iter + 1) & (hash_map->capacity - 1); 
    }
    return NULL;
}

static void hash_map_print(hash_map_t* hash_map) {
    for (size_t i = 0; i < hash_map->capacity; i++) {
        entry_t* entry = *(hash_map->entries + i); 
        if (entry != NULL && !entry->tombstone) {
            printf("%s\n", entry->key); 
        }
    }
}

static void hash_map_free(hash_map_t* hash_map) {
    for (size_t i = 0; i < hash_map->capacity; i++) {
        entry_t* entry = *(hash_map->entries + i);
        if (entry != NULL)
            free(entry);
    }
    free(hash_map->entries);
    hash_map->entries = NULL; 
}

#endif