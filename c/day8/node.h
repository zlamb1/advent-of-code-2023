#ifndef NODE_H
#define NODE_H

#include "../array.h"
#include "../hash_map.h"

MAKE_ARRAY(char)

typedef struct node {
    char_array_t name;
    char_array_t paths[2]; 
    size_t steps;
} node_t; 

MAKE_MAP(node, char*, node_t)

#endif