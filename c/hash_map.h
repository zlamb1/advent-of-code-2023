#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmp.h"
#include "format.h"

// general-purpose hash-map implementation that uses a power of two capacity and linear-probing

#ifndef __HASH_TABLE_INIT_CAPACITY
#define __HASH_TABLE_INIT_CAPACITY 16
#endif

#ifndef __HASH_TABLE_LOAD_FACTOR
#define __HASH_TABLE_LOAD_FACTOR 0.75
#endif

typedef enum HASH_MAP_RESULT {
    HASH_MAP_RESULT_SUCCESS         =  0,
    HASH_MAP_RESULT_NULL_ENTRIES    = -1,
    HASH_MAP_RESULT_OUT_OF_MEMORY   = -2,
    HASH_MAP_RESULT_NO_CAPACITY     = -3,
    HASH_MAP_RESULT_NULL_HASH_FN    = -4,
    HASH_MAP_RESULT_NULL_KEY_CMP_FN = -5,
    HASH_MAP_RESULT_NOT_FOUND       = -6
} HASH_MAP_RESULT_T;

static const char* __hash_map_result_to_str(HASH_MAP_RESULT_T result) {
    switch (result) {
        case HASH_MAP_RESULT_SUCCESS:
            return "SUCCESS";         
        case HASH_MAP_RESULT_NULL_ENTRIES:
            return "NULL_ENTRIES";    
        case HASH_MAP_RESULT_OUT_OF_MEMORY:
            return "OUT_OF_MEMORY";   
        case HASH_MAP_RESULT_NO_CAPACITY:
            return "NO_CAPACITY";      
        case HASH_MAP_RESULT_NULL_HASH_FN:
            return "NULL_HASH_FN";    
        case HASH_MAP_RESULT_NULL_KEY_CMP_FN:
            return "NULL_KEY_CMP_FN"; 
        case HASH_MAP_RESULT_NOT_FOUND:
            return "NOT_FOUND";      
        default:
            return "UNKNOWN";
    }
}

static void* __hash_map_calloc(size_t num_elements, size_t size, void* ctx) {
    return calloc(num_elements, size);
}

static void __hash_map_free(void* ptr, size_t size, void* ctx) {
    free(ptr); 
}

#define __MAKE_HASH_FN(NAME, TYPE)             \
static unsigned long __hash_##NAME(TYPE key) { \
    unsigned long hash = 5381;                 \
    return ((hash << 5) + hash) + key;         \
}

__MAKE_HASH_FN(uchar,  unsigned char)
__MAKE_HASH_FN(char,   char)
__MAKE_HASH_FN(schar,  signed char)
__MAKE_HASH_FN(ushort, unsigned short)
__MAKE_HASH_FN(short,  short)
__MAKE_HASH_FN(uint,   unsigned int)
__MAKE_HASH_FN(int,    int)
__MAKE_HASH_FN(ulong,  unsigned long)
__MAKE_HASH_FN(long,   long)

static unsigned long __hash_str(char* key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + (c + 128);
    return hash;
}

#define __HASH_FN(X) _Generic((X),                                                    \
    unsigned char:  &__hash_uchar,  char:  &__hash_char, signed char:  &__hash_schar, \
    unsigned short: &__hash_ushort, short: &__hash_short,                             \
    unsigned int:   &__hash_uint,   int:   &__hash_int,                               \
    unsigned long:  &__hash_ulong,  long:  &__hash_long,                              \
    unsigned char*: &__hash_str,    char*: &__hash_str,  signed char*: &__hash_str,   \
    default: NULL                                                                     \
)

#define MAKE_MAP(NAME, KEY_TYPE, VAL_TYPE)                                                                  \
typedef struct NAME##_entry {                                                                               \
    KEY_TYPE key;                                                                                           \
    unsigned long hash;                                                                                     \
    VAL_TYPE value;                                                                                         \
    bool tombstone;                                                                                         \
} NAME##_entry_t;                                                                                           \
typedef struct NAME##_map {                                                                                 \
    NAME##_entry_t** entries;                                                                               \
    size_t num_entries, capacity;                                                                           \
    void* alloc_ctx;                                                                                        \
    void* (*calloc_fn)(size_t num_elements, size_t size, void* ctx);                                        \
    void (*free_fn)(void* ptr, size_t size, void* ctx);                                                     \
    unsigned long (*hash_fn)(KEY_TYPE key);                                                                 \
    int (*key_cmp_fn)(KEY_TYPE a, KEY_TYPE b);                                                              \
} NAME##_map_t;                                                                                             \
static HASH_MAP_RESULT_T NAME##_map_init(NAME##_map_t* map) {                                               \
    map->num_entries = 0;                                                                                   \
    map->capacity    = __HASH_TABLE_INIT_CAPACITY;                                                          \
    map->calloc_fn   = &__hash_map_calloc;                                                                  \
    map->free_fn     = &__hash_map_free;                                                                    \
    KEY_TYPE x;                                                                                             \
    map->hash_fn     = __HASH_FN(x);                                                                        \
    map->key_cmp_fn  = __CMP_FN(x);                                                                         \
    map->entries     = map->calloc_fn(__HASH_TABLE_INIT_CAPACITY, sizeof(NAME##_entry_t*), map->alloc_ctx); \
    if (map->entries == NULL)                                                                               \
        return HASH_MAP_RESULT_OUT_OF_MEMORY;                                                               \
    return HASH_MAP_RESULT_SUCCESS;                                                                         \
}                                                                                                           \
static HASH_MAP_RESULT_T NAME##_map_alloc(NAME##_map_t* map) {                                              \
    if (map->entries == NULL)                                                                               \
        return HASH_MAP_RESULT_NULL_ENTRIES;                                                                \
    if (map->hash_fn == NULL)                                                                               \
        return HASH_MAP_RESULT_NULL_HASH_FN;                                                                \
    size_t old_capacity = map->capacity;                                                                    \
    while ((double)map->num_entries / (double)map->capacity > __HASH_TABLE_LOAD_FACTOR)                     \
        map->capacity *= 2;                                                                                 \
    NAME##_entry_t** entries = map->calloc_fn(map->capacity, sizeof(NAME##_entry_t*), map->alloc_ctx);      \
    size_t counter = 0;                                                                                     \
    /* move old hashes into new number range */                                                             \
    for (size_t i = 0; i < old_capacity; i++) {                                                             \
        NAME##_entry_t* entry = *(map->entries + i);                                                        \
        if (entry != NULL) {                                                                                \
            if (entry->tombstone) {                                                                         \
                map->free_fn(entry, sizeof(NAME##_entry_t*), map->alloc_ctx);                               \
                continue;                                                                                   \
            }                                                                                               \
            /* fast modulo when capacity is a power of 2 */                                                 \
            size_t index = entry->hash & (map->capacity - 1), iter = index;                                 \
            while (*(entries + iter) != NULL) {                                                             \
                if (iter == index - 1) {                                                                    \
                    /* wrapped-around and found no empty spot? */                                           \
                    map->free_fn(entries, sizeof(NAME##_entry_t*) * map->capacity, map->alloc_ctx);         \
                    return HASH_MAP_RESULT_NO_CAPACITY;                                                     \
                }                                                                                           \
                iter = (iter + 1) & (map->capacity - 1);                                                    \
            }                                                                                               \
            *(entries + iter) = entry;                                                                      \
            counter++;                                                                                      \
        }                                                                                                   \
    }                                                                                                       \
    map->free_fn(map->entries, sizeof(NAME##_entry_t*) * old_capacity, map->alloc_ctx);                     \
    map->entries = entries;                                                                                 \
    map->num_entries = counter;                                                                             \
    return HASH_MAP_RESULT_SUCCESS;                                                                         \
}                                                                                                           \
static HASH_MAP_RESULT_T NAME##_map_insert(NAME##_map_t* map, KEY_TYPE key, VAL_TYPE value) {               \
    if (map->key_cmp_fn == NULL)                                                                            \
        return HASH_MAP_RESULT_NULL_KEY_CMP_FN;                                                             \
    HASH_MAP_RESULT_T result = NAME##_map_alloc(map);                                                       \
    if (result != HASH_MAP_RESULT_SUCCESS)                                                                  \
        return result;                                                                                      \
    unsigned long key_hash = map->hash_fn(key);                                                             \
    size_t index = key_hash & (map->capacity - 1), iter = index;                                            \
    NAME##_entry_t* p_entry;                                                                                \
    while ((p_entry = *(map->entries + iter)) != NULL) {                                                    \
        if (key_hash == p_entry->hash && map->key_cmp_fn(key, p_entry->key) == 0)                           \
            break;                                                                                          \
        if (iter == index - 1)                                                                              \
            return HASH_MAP_RESULT_NO_CAPACITY;                                                             \
        iter = (iter + 1) & (map->capacity - 1);                                                            \
    }                                                                                                       \
    NAME##_entry_t* new_entry = map->calloc_fn(1, sizeof(NAME##_entry_t), map->alloc_ctx);                  \
    new_entry->key = key;                                                                                   \
    new_entry->hash = key_hash;                                                                             \
    new_entry->value = value;                                                                               \
    *(map->entries + iter) = new_entry;                                                                     \
    map->num_entries++;                                                                                     \
    return HASH_MAP_RESULT_SUCCESS;                                                                         \
}                                                                                                           \
static HASH_MAP_RESULT_T NAME##_map_lookup(NAME##_map_t* map, KEY_TYPE key, VAL_TYPE* out) {                \
    if (map->entries == NULL)                                                                               \
        return HASH_MAP_RESULT_NULL_ENTRIES;                                                                \
    if (map->hash_fn == NULL)                                                                               \
        return HASH_MAP_RESULT_NULL_HASH_FN;                                                                \
    if (map->key_cmp_fn == NULL)                                                                            \
        return HASH_MAP_RESULT_NULL_KEY_CMP_FN;                                                             \
    unsigned long key_hash = map->hash_fn(key);                                                             \
    size_t index = key_hash & (map->capacity - 1), iter = index;                                            \
    NAME##_entry_t* entry;                                                                                  \
    while ((entry = *(map->entries + iter)) != NULL) {                                                      \
        if (!entry->tombstone && key_hash == entry->hash && map->key_cmp_fn(key, entry->key) == 0) {        \
            *out = entry->value;                                                                            \
            return HASH_MAP_RESULT_SUCCESS;                                                                 \
        }                                                                                                   \
        if (iter == index - 1)                                                                              \
            break;                                                                                          \
        iter = (iter + 1) & (map->capacity - 1);                                                            \
    }                                                                                                       \
    return HASH_MAP_RESULT_NOT_FOUND;                                                                       \
}                                                                                                           \
static HASH_MAP_RESULT_T NAME##_map_delete(NAME##_map_t* map, KEY_TYPE key, VAL_TYPE* out) {                \
    if (map->entries == NULL)                                                                               \
        return HASH_MAP_RESULT_NULL_ENTRIES;                                                                \
    if (map->hash_fn == NULL)                                                                               \
        return HASH_MAP_RESULT_NULL_HASH_FN;                                                                \
    if (map->key_cmp_fn == NULL)                                                                            \
        return HASH_MAP_RESULT_NULL_KEY_CMP_FN;                                                             \
    unsigned long key_hash = map->hash_fn(key);                                                             \
    size_t index = key_hash & (map->capacity - 1), iter = index;                                            \
    NAME##_entry_t* entry;                                                                                  \
    while ((entry = *(map->entries + iter)) != NULL) {                                                      \
        if (!entry->tombstone && key_hash == entry->hash && map->key_cmp_fn(key, entry->key) == 0) {        \
            map->num_entries--;                                                                             \
            entry->tombstone = true;                                                                        \
            if (out != NULL)                                                                                \
                *out = entry->value;                                                                        \
            return HASH_MAP_RESULT_SUCCESS;                                                                 \
        }                                                                                                   \
        if (iter == index - 1)                                                                              \
            break;                                                                                          \
        iter = (iter + 1) & (map->capacity - 1);                                                            \
    }                                                                                                       \
    return HASH_MAP_RESULT_NOT_FOUND;                                                                       \
}                                                                                                           \
static void NAME##_map_free(NAME##_map_t* map) {                                                            \
    for (size_t i = 0; i < map->capacity; i++) {                                                            \
        NAME##_entry_t* entry = *(map->entries + i);                                                        \
        if (entry != NULL)                                                                                  \
            map->free_fn(entry, sizeof(NAME##_entry_t*), map->alloc_ctx);                                   \
    }                                                                                                       \
    map->free_fn(map->entries, sizeof(NAME##_entry_t*) * map->capacity, map->alloc_ctx);                    \
    map->entries = NULL;                                                                                    \
}

#define __MAKE_MAP_PRINT_FN(NAME, KEY_TYPE, VAL_TYPE)                               \
static void NAME##_map_print(NAME##_map_t* map) {                                   \
    KEY_TYPE x;                                                                     \
    VAL_TYPE y;                                                                     \
    printf("{");                                                                    \
    for (size_t i = 0; i < map->capacity; i++) {                                    \
        NAME##_entry_t* entry = *(map->entries + i);                                \
        if (entry != NULL && !entry->tombstone) {                                   \
            printf(FORMAT_SPECIFIER(x, ": "), entry->key);                          \
            printf(FORMAT_SPECIFIER(y, "\n"), entry->value);                        \
        }                                                                           \
    }                                                                               \
    printf("}\n");                                                                  \
}        

#endif