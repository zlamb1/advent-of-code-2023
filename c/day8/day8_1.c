#include "../str.h"
#include "../hash_map.h"

static const char* file_name = "day8.txt";

int main(void) {
    file_content_t file_content = {0};
    int err_code = read_file(&file_content, file_name);
    if (err_code != FILE_READ_SUCCESS) {
        print_file_read_code(err_code, file_name);
        return EXIT_FAILURE;
    }
    hash_map_t hash_map = {0};
    hash_map_init(&hash_map);
    printf("%i\n", 0); 
    return 0;
}