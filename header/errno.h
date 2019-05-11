#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INCLUDED

typedef enum {
    SYNTAX_ERR,
    LACK_OF_OP,
    TOO_MUCH_OPS,
    LACK_OF_PARENT,
    UNMATCH_TYPES,
    UNKNOWN_ERR,
    SYMBOL_UNKNOWN,
} errno;

#endif