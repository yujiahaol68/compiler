#include <stdio.h>
#include "../header/lexer.h"
#include "../header/global.h"
#include "assert.h"
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
        //assert(tk.kind == IDENT_TOKEN);
    }
}

void init() {
    g_line_num=0;
}

int main() {
    printf("%d", EMPTY_SYM);
    init();
    lexer_init();
    char buf[1024];
    while (fgets(buf, 1024, stdin) != NULL) {
        ++g_line_num;
        parse_line(buf);
    }
    return 0;
}