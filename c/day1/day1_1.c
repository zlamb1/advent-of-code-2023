#include "../str.h"

static const char* file_name = "day1.txt";

int main(void) {
    file_content_t file_content = {};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    int accum = 0;
    for (size_t i = 0; i < file_content.num_lines; i++) {
        int sum = 0, first_num = -1, last_num = -1;
        line_t* line = *(file_content.lines + i); 
        for (size_t j = 0; j < line->len; j++) {
            int num = *(line->str + j) - '0';
            if (num > -1 && num < 10) {
                if (first_num == -1)
                    first_num = num;
                last_num = num; 
            }
        }
        if (first_num > -1) {
            sum = first_num * 10 + last_num;
        }
        accum += sum;
    }
    printf("%i\n", accum);
    content_free(&file_content);
    return EXIT_SUCCESS;
}