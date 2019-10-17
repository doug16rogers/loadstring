#include <stdio.h>

extern int handle_buffer(const char *s, int (*handler)(const char *));

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s: expect a single string argument\n", argv[0]);
        return 1;
    }
    handle_buffer(argv[1], puts);
    return 0;
}
