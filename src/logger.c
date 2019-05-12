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
            break;
        case LACK_OF_OP:
            msg = "syntax error: lack of OPs\n";
            break;
        case LACK_OF_PARENT:
            msg = "syntax error: lack of parenthesis\n";
            break;
        case TOO_MUCH_OPS:
            msg = "syntax error: too much OPs\n";
            break;
        case UNMATCH_TYPES:
            msg = "syntax error: found unmatched type when generate code\n";
            break;
        default:
            break;
    }
    if (msg != NULL)
        fprintf(stderr, msg);
    if (str)
        fprintf(stderr, str);
}