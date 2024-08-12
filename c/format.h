#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>

#define FORMAT_SPECIFIER(T, P) _Generic((T), \
    char:          "%c"  P, char*:  "%s"  P,     \
    unsigned int:  "%u"  P, int:    "%i"  P,     \
    unsigned long: "%lu" P, long:   "%ld" P,     \
    float:         "%f"  P, double: "%d"  P,     \
    default:       ""    P                       \
)

static void __print_void_ptr(void* ptr) {
    printf("%p", ptr); 
}

static void __print_str(char* str) {
    printf("%s", str);
}

#endif