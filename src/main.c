#include <stdio.h>
#include "../header/lexer.h"
#include "../header/global.h"
#include "assert.h"
#include "../header/parser.h"
#include "../header/hashtable.h"
#include <string.h>

int g_line_num;

void parse_line(char *buf) {
    Token tk;
    reset_line(buf);

    while(1) {
        get_token(&tk);
        if (tk.kind == END_LINE_TOKEN) break;

        printf("Kind .. %d, str .. %s\n", tk.kind, tk.str);

        memset(tk.str, '\0', sizeof(tk.str));
    }
}

void init() {
    g_line_num=0;
}

int Test_Lexer_main() {
    init();
    lexer_init();
    char buf[1024];
    while (fgets(buf, 1024, stdin) != NULL) {
        ++g_line_num;
        parse_line(buf);
    }
    return 0;
}

int main() {
    g_line_num = 1;
    lexer_init();
    init_sym_table();

    char buf[1024];
    while(fgets(buf, 1024, stdin) != NULL) {
        reset_line(buf);
        parse();
    }

    printf("Grammar analysis finish !");
    destroy_sym_table();
    return 0;
}