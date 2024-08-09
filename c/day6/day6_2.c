#include "math.h"

#include "../str.h"
#include "race.h"

static const char* file_name = "day6.txt";

int parse_races(file_content_t* file_content) {
    race_list_t race_list;
    race_list_init(&race_list);
    long counter = 0, product = 1;
    if (file_content->num_lines < 2)
        return 0;
    line_t* line = *file_content->lines; 
    ASSERT_STR_CMP(line, "Time:");
    while (line_get_pos_char(line) != '\0') {
        race_t race = {}; 
        CONSUME_WHITESPACE(line);
        line_parse_long_ignore_whitespace(line, &race.time); 
        CONSUME_WHITESPACE(line);
        race_list_append(&race_list, race);
    }
    line = *(file_content->lines + 1); 
    ASSERT_STR_CMP(line, "Distance:");
    while (line_get_pos_char(line) != '\0') {
        if (counter >= race_list.len) {
            // panic!
            fprintf(stderr, "err: mismatch in time and distance numbers\n");
            return 0;
        }
        race_t* race = race_list.races + counter;
        CONSUME_WHITESPACE(line);
        line_parse_long_ignore_whitespace(line, &race->distance); 
        CONSUME_WHITESPACE(line);
        counter++;
    }
    for (size_t i = 0; i < race_list.len; i++) {
        race_t* race = race_list.races + i; 
        double half_time = race->time / 2.0;
        double a = -1, b = race->time,  c = -race->distance; 
        double first = floor((-b - sqrt(pow(b, 2) - 4 * a * c)) / 2 * a), second = ceil((-b + sqrt(pow(b, 2) - 4 * a * c)) / 2 * a);
        product *= first - second + 1;
    }
    race_list_free(&race_list);
    return product;
}

int main(void) {
    file_content_t file_content = {};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    printf("%i\n", parse_races(&file_content)); 
    return 0;
}