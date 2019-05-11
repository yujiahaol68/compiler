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

void const_decl();
void var_decl();
void get_next_token();

void parse();
double parse_expr();
double parse_expr_tail(double lvalue);
double parse_term();
double parse_term_tail(double lvalue);
double parse_factor();

void print_line_info();

void must_match(TokenKind k);
void go_match(TokenKind k);
int is_cal_ops(TokenKind k);
int is_boundary(TokenKind k);
TokenKind match_type();

#endif //COMPILER_PARSER_H
