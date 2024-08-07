#include "../str.h"

static const char* file_name = "day2.txt";

typedef struct game {
    int id, color_buf[3]; 
    bool possible; 
} game_t; 

static const char* colors[] = {
    "red", "blue", "green"
};

int fmaxi(int a, int b) {
    return a > b ? a : b;
}

game_t parse_game(line_t* line) {
    game_t game = {};
    for (size_t i = 0; i < 3; i++)
        game.color_buf[i] = 0; 
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
                line->pos++;
                CONSUME_WHITESPACE(line);
                for (size_t i = 0; i < 3; i++) 
                    game.color_buf[i] = fmaxi(game.color_buf[i], buf[i]);
                memset(&buf, 0, sizeof(int) * 3);
                break;
            case '\0':
                for (size_t i = 0; i < 3; i++) 
                    game.color_buf[i] = fmaxi(game.color_buf[i], buf[i]);
                memset(&buf, 0, sizeof(int) * 3);
                break;
            default:
                fprintf(stderr, "unexpected char '%c'\n", cur_char); 
                exit(EXIT_FAILURE);
        }
    }
    return game; 
}

bool is_game_valid(game_t* game) {
    printf("game %i is ", game->id);
    int max_buf[3] = { 12, 14, 13 };
    for (size_t i = 0; i < 3; i++) {
        if (game->color_buf[i] > max_buf[i]) {
            printf("invalid\n");
            return false;
        }
    }
    printf("valid\n");
    return true;
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
        if (is_game_valid(&game))
            accum += game.id; 
    }
    printf("accum: %i\n", accum);
    content_free(&file_content);
    return EXIT_SUCCESS;
}