//
// Created by Tecker on 2019-04-19.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer.h"

static Token g_cur_tk;

typedef enum {
    STRING_TYPE,
    NUMBER_TYPE,
    BOOL_TYPE,
} DataType;

typedef enum {
    END_OF_BLOCK,
    END_OF_LINE,
    SYNTAX_ERROR
} Cause;

struct Element {
    DataType t;
    TokenKind k;
    // 字符串常量或数值或变量名
    char name[101]; // 为空的时候表示是字符串常量或者数字
    // int, double, bool:0,1
    double number;
};

struct Element get_Elem(Token t);
void genByElement(TokenKind k, struct Element* first, struct Element* second);

void var_assign(char* var_name); // 变量赋值
void const_decl(); // 常量声明
void var_decl(); // 变量声明
void condition_stmt(const int* true_out);
void get_next_token();

Cause parse();

struct Element parse_expr();
struct Element parse_expr_tail(struct Element* first);
struct Element parse_term();
struct Element parse_term_tail(struct Element* first);
struct Element parse_factor();

struct Element parse_bool_expr(int* pre_t, int* pre_f);

void print_line_info();

void must_match(TokenKind k);
void go_match(TokenKind k);
int is_cal_ops(TokenKind k);
int is_boundary(TokenKind k);
int is_condi_ops(TokenKind k);
TokenKind match_type();

#endif //COMPILER_PARSER_H
