#include "../str.h"

static const char* file_name = "day1.txt";

static const char* digits[9] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

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
        int sum = 0, first_num = -1, last_num = -1;
        for (; line->pos < line->len; line->pos++) {
            int num = *(line->str + line->pos) - '0';
            if (num > 0 && num < 10) {
                if (first_num == -1)
                    first_num = num;
                last_num = num; 
            } else {
                int digit = line_const_cmp_str_arr(line, digits, 9); 
                if (digit > -1) {
                    digit += 1; 
                    if (first_num == -1)
                        first_num = digit; 
                    last_num = digit; 
                }
            }
        }
        if (first_num > -1) {
            sum = first_num * 10 + last_num;
            //printf("line %li : %i\n", line->line_num, sum);
        }
        accum += sum;
    }
    printf("%i\n", accum);
    content_free(&file_content);
    return EXIT_SUCCESS;
}