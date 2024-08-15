#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../str_buf.h"
#include "galaxy.h"

static const char* file_name = "day11.txt";

void insert_row(size_t row, str_buf_t* buf) {
    for (size_t i = 0; i < buf->max_line_len; i++) {
        str_buf_insert(buf, '.', buf->max_line_len * row); 
    }
}

void insert_col(size_t col, str_buf_t* buf) {
    size_t col_size = buf->len / buf->max_line_len + 1;
    for (size_t i = 0; i < col_size; i++) {
        str_buf_insert(buf, '.', buf->max_line_len * i + i + col); 
    }
    buf->max_line_len++;
}

void find_distance(galaxy_pair* pair, size_t num_cols) {
    galaxy* g1 = pair->galaxies[0], *g2 = pair->galaxies[1]; 
    size_t row1 = g1->index / num_cols, row2 = g2->index / num_cols, col1 = g1->index % num_cols, col2 = g2->index % num_cols;
    size_t distance = MAX(row1, row2) - MIN(row1, row2) + MAX(col1, col2) - MIN(col1, col2);
    pair->distance = distance;
}


size_t expand_buf(str_buf_t* buf) {
    size_t num_rows = buf->len / buf->max_line_len, num_cols = buf->max_line_len; 
    bool* rows = malloc(sizeof(bool) * num_rows);
    memset(rows, 1, sizeof(bool) * num_rows);
    bool* cols = malloc(sizeof(bool) * num_cols);
    memset(cols, 1, sizeof(bool) * num_cols);
    for (size_t i = 0; i < buf->len; i++) {
        size_t row = i / num_cols, col = i % num_cols;
        if (*(buf->data + i) != '.') {
            rows[row] = false;
            cols[col] = false;
        }
    }
    size_t row_counter = 0, col_counter = 0;
    for (size_t i = 0; i < (num_rows > num_cols ? num_rows : num_cols); i++) {
        if (i < num_rows && rows[i]) {
            insert_row(i + row_counter, buf);
            row_counter++;
        }
        if (i < num_cols && cols[i]) {
            insert_col(i + col_counter, buf);
            col_counter++;
        }
    }
    free(rows);
    free(cols);
    galaxy_array_t array;
    galaxy_array_init(&array);
    galaxy_pair_array_t pair_array; 
    galaxy_pair_array_init(&pair_array);
    size_t counter = 1, sum = 0;
    num_cols = buf->max_line_len; 
    for (size_t i = 0; i < buf->len; i++) {
        if (buf->data[i] == '#') {
            galaxy g = {.id = counter, .index = i}; 
            galaxy_array_append(&array, g); 
            for (size_t i = 0; i < array.len; i++) {
                if (array.data[i].id != counter) {
                    galaxy_pair pair = {.galaxies = { &array.data[array.len - 1], &array.data[i] }};
                    find_distance(&pair, num_cols);
                    sum += pair.distance;
                    galaxy_pair_array_append(&pair_array, pair);
                }
            }
            counter++;
        }
    }
    galaxy_pair_array_free(&pair_array);
    galaxy_array_free(&array);
    return sum;
}

void print_buf(str_buf_t* buf) {
    for (size_t i = 0; i < buf->len; i++) {
        printf("%c", *(buf->data + i));
        if ((i + 1) % buf->max_line_len == 0) {
            printf("\n");
        }
    }
}

int main(void) {
    str_buf_t buf;
    FILE_READ_CODE_T err_code = read_file(file_name, &buf);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return 1; 
    }
    printf("%ld\n", expand_buf(&buf));
    str_buf_free(&buf);
    return 0;
}