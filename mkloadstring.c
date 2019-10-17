#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *g_prog = "mkloadstring";
const char *g_reg = "rdi";
int g_push = 0;

void usage(FILE *f, int exit_code) {
    fprintf(f, "\n");
    fprintf(f, "    Usage: %s [-r reg] [-p] <string>\n", g_prog);
    fprintf(f, "\n");
    exit(exit_code);
}

void emit_load_string_asm(const uint8_t *s) {
    if (g_push)
        printf("\tpush\t%%%s\n", g_reg);
    for (; *s; s++) {
        printf("\tmovb\t$0x%02x, (%%%s)\n", *s, g_reg);
        printf("\tinc\t%%%s\n", g_reg);
    }
    printf("\tmovb\t$0x00, (%%%s)\n", g_reg);
    if (g_push)
        printf("\tpop\t%%%s\n", g_reg);
}

int main(int argc, char *argv[]) {
    int ch;
    while  ((ch = getopt(argc, argv, "hpr:")) != EOF) {
        switch (ch) {
        case 'h': usage(stdout, 0); break;
        case 'r': g_reg = optarg; break;
        case 'p': g_push = 1; break;
        default:
            fprintf(stderr, "%s: unknown option '%c'\n", g_prog, ch);
            usage(stderr, 1);
        }
    }
    argc -= optind;
    argv += optind;
    if (argc != 1) {
        fprintf(stderr, "%s: must supply single string argument\n", g_prog);
        usage(stderr, 1);
    }
    emit_load_string_asm((const uint8_t *) argv[0]);
    return 0;
}

