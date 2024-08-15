#include <stdbool.h>
#include <string.h>

#include "../str_buf.h"
#include "pipe.h"

static const char* file_name = "day10.txt";

bool is_facing(char c, direction dir) {
    switch (c) {
        case 'S':
            return true;
        case '|':
            return dir == SOUTH || dir == NORTH;
        case '-':
            return dir == WEST || dir == EAST;
        case 'L':
            return dir == SOUTH || dir == WEST;
        case 'J':
            return dir == SOUTH || dir == EAST;
        case 'F':
            return dir == NORTH || dir == WEST;
        case '7':
            return dir == NORTH || dir == EAST;
        default:
            return false;
    }
}

void get_compat_directions(char c, bool* compat_array) {
    switch (c) {
        case 'S':
            memset(compat_array, true, sizeof(bool) * 4); 
            break;
        case '|':
            compat_array[0] = true;
            compat_array[3] = true;
            break;
        case '-':
            compat_array[1] = true;
            compat_array[2] = true;
            break;
        case 'L':
            compat_array[0] = true;
            compat_array[2] = true;
            break;
        case 'J':
            compat_array[0] = true;
            compat_array[1] = true;
            break;
        case 'F':
            compat_array[2] = true;
            compat_array[3] = true;
            break;
        case '7':
            compat_array[1] = true;
            compat_array[3] = true;
            break;
    }
}

pipe* parse_neighbors(pipe** array, pipe* p, str_buf_t* buf) {
    static direction directions[4] = { NORTH, WEST, EAST, SOUTH };
    bool compat_array[4] = {0};
    get_compat_directions(p->c, compat_array);
    int offsets[4] = { -buf->max_line_len, -1, 1, buf->max_line_len };
    for (size_t i = 0; i < 4; i++) {
        int index = p->index + offsets[i];
        if (compat_array[i] && index > -1 && index < buf->len) {
            if (p->prev == NULL || p->prev->index != index) {
                char c = *(buf->data + index);
                if (is_facing(c, directions[i])) {
                    if (*(array + index) != NULL) {
                        pipe* f = *(array + index);
                        f->prev = p; 
                        p->next = f;
                        return NULL;
                    } else {
                        pipe* next = calloc(1, sizeof(pipe));
                        next->c = c; 
                        next->prev = p;
                        next->index = index;
                        *(array + index) = next; 
                        p->next = *(array + index); 
                        return next; 
                    }
                }
            }
        }
    }
    return NULL;
}

int parse_pipes(str_buf_t* buf) {
    pipe** array = calloc(buf->len, sizeof(pipe*));
    pipe* start = calloc(1, sizeof(pipe));
    start->c = 'S';
    start->is_start = 1;  
    for (size_t i = 0; i < buf->len; i++) {
        if (*(buf->data + i) == 'S') {
            start->index = i;
            *(array + i) = start; 
            break;
        }
    }
    pipe* cur = start;
    while ((cur = parse_neighbors(array, cur, buf)) != NULL) {
        if (cur->is_start) break;
    }
    size_t counter = 1;
    cur = start;
    while ((cur = cur->next) != NULL && !cur->is_start) {
        cur->dist[0] = counter;
        counter++;
    }
    counter = 1; 
    cur = start;
    while ((cur = cur->prev) != NULL && !cur->is_start) {
        cur->dist[1] = counter;
        counter++;
    }
    int max_dist = -1; 
    for (size_t i = 0; i < buf->len; i++) {
        pipe* p = *(array + i); 
        if (p != NULL) {
            int min = p->dist[0] > p->dist[1] ? p->dist[1] : p->dist[0];
            if (min > max_dist) max_dist = min;
            free(p); 
        }
    }
    free(array);
    return max_dist;
}

int main(void) {
    str_buf_t buf; 
    read_file(file_name, &buf);
    printf("%i\n", parse_pipes(&buf));
    str_buf_free(&buf);
    return 0;
}