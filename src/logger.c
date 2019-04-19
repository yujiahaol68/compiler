#include "../header/logger.h"
#include <stdio.h>

void print_err(errno n, const char* str) {
    char * msg = NULL;
    switch (n)
    {
        case SYNTAX_ERR:
            msg = "syntax error\n";
            break;
        case UNKNOWN_ERR:
            msg = "unknown error\n";
        default:
            break;
    }
    if (msg != NULL)
        fprintf(stderr, msg);
    if (str)
        fprintf(stderr, str);
}