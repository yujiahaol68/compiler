//
// Created by Tecker on 2019-05-11.
//

#ifndef COMPILER_HASHTABLE_H
#define COMPILER_HASHTABLE_H

#include "parser.h"

struct Sym {
    DataType  t;
    void *    val;
};

typedef enum {
    OK,
    DUP_KEY,
} ht_err;

ht_err symtable_reg_Var(char* name, DataType t, void* val);

struct Sym* symtable_get_by_name(char* name);
void remove_sym(char* name);

void init_sym_table();
void destroy_sym_table();

#endif //COMPILER_HASHTABLE_H
