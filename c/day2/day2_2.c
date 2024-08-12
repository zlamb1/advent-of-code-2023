#include "../str.h"

#include <limits.h>

static const char* file_name = "day2.txt";

typedef struct game {
    int id, power, color_buf[3];
    bool possible; 
} game_t; 

static const char* colors[] = {
    "red", "blue", "green"
};

int fmini(int a, int b) {
    return a < b ? a : b;
}

int fmaxi(int a, int b) {
    return a > b ? a : b;
}

game_t parse_game(line_t* line) {
    game_t game = {};
    for (size_t i = 0; i < 3; i++)
        game.color_buf[i] = INT_MIN;
    ASSERT_STR_CMP(line, "Game");
    CONSUME_WHITESPACE(line); 
    ASSERT_STR_INT(line, &game.id);
    ASSERT_STR_CMP(line, ":"); 
    CONSUME_WHITESPACE(line); 
    int num, buf[3];
    memset(&buf, 0, sizeof(int) * 3); 
    char cur_char = line_get_pos_char(line); 
    while (cur_char != '\0') {
        if (line_get_pos_char(line) == '\0')
            break;
        ASSERT_STR_INT(line, &num); 
        CONSUME_WHITESPACE(line);
        int color = line_cmp_str_arr(line, colors, 3);
        buf[color] += num; 
        cur_char = line_get_pos_char(line); 
        switch (cur_char) {
            case ',':
                line->pos++;
                CONSUME_WHITESPACE(line);
                break;
            case ';': 
            case '\0':
                if (cur_char == ';') {
                    line->pos++;
                    CONSUME_WHITESPACE(line); 
                }
                for (size_t i = 0; i < 3; i++) {
                    game.color_buf[i] = fmaxi(game.color_buf[i], buf[i]);
                }
                memset(&buf, INT_MIN, sizeof(int) * 3);
                break;
            default:
                fprintf(stderr, "unexpected char '%c'\n", cur_char); 
                exit(EXIT_FAILURE);
        }
    }
    game.power = game.color_buf[0] * game.color_buf[1] * game.color_buf[2]; 
    return game; 
}

int main(void) {
    file_content_t file_content = {};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    int accum = 0;
    for (size_t i = 0; i < file_content.num_lines; i++) {
        line_t* line = *(file_content.lines + i); 
        game_t game = parse_game(line); 
        accum += game.power; 
    }
    printf("%i\n", accum);
    return EXIT_SUCCESS;
}