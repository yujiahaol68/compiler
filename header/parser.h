//
// Created by Tecker on 2019-04-19.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer.h"

static Token g_cur_tk;

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
