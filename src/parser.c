//
// Created by Tecker on 2019-04-19.
//

#include "../header/parser.h"
#include "../header/logger.h"
#include "../header/global.h"
#include <stdio.h>
#include <stdlib.h>

void print_line_info() {
    char msg[15];
    sprintf(msg, "Line %d:%d", g_line_num, get_line_pos());
    printf("%s\n", msg);
}

void get_next_token() {
    get_token(&g_cur_tk);
}

void go_match(TokenKind k) {
    get_next_token();
    must_match(k);
}

void must_match(TokenKind k) {
    if (g_cur_tk.kind == k) {
        return;
    }
    char msg[50];
    sprintf(msg, "Line %d:%d Expect %s But got %s\n", g_line_num, get_line_pos(), t_to_string(k), t_to_string(g_cur_tk.kind));
    print_err(SYNTAX_ERR, msg);
    g_line_err = 1;
    //exit(1);
}

int is_match(TokenKind k) {
    return g_cur_tk.kind == k;
}

int is_cal_ops(TokenKind k) {
    switch (k) {
        case ADD_OP_TOKEN:
        case SUB_OP_TOKEN:
        case DIV_OP_TOKEN:
        case MUL_OP_TOKEN:
            return 1;
        default:
            return 0;
    }
}

// { \n ;
int is_boundary(TokenKind k) {
    switch (k) {
        case SEMICOLON_TOKEN:
        case LBRACKET_TOKEN:
        case END_LINE_TOKEN:
            return 1;
        default:
            return 0;
    }
}

void print_tk() {
    printf("Kind: %d, Val: %s\n", g_cur_tk.kind, g_cur_tk.str);
}

void parse() {
    while(1) {
        get_next_token();
        if (g_cur_tk.kind == END_LINE_TOKEN) {
            ++g_line_num;
            get_next_token();
            return;
        }

        switch (g_cur_tk.kind) {
            // var a int = 15
            // var b int = 1+2
            // var c int = (b - a) * a / b
            case VAR_SYM:
                var_decl();
                break;
            case IDENT_TOKEN:
                get_next_token();
                // a := 3.14
                // b := true
                if (is_match(BECOMES_TOKEN)) {
                    get_next_token();
                    parse_expr();
                }
                // a = 4
                // b = 6
                else if (is_match(EQL_TOKEN)) {
                    // TODO: look up symbol table
                    get_next_token();
                    parse_expr();
                }
                break;
            case CONST_SYM:
                const_decl();
                break;
            default:
                //print_err(SYNTAX_ERR, " Or not implement yet!");
                //exit(1);
                // auto skip
                break;
        }
        if (g_line_err) {
            // fast skip
            while (g_cur_tk.kind != END_LINE_TOKEN) {
                get_next_token();
            }
            ++g_line_num;
            get_next_token();
            g_line_err = 0;
            return;
        }
    }
}

void var_decl() {
    go_match(IDENT_TOKEN);
    if (match_type() != NUL_TOKEN) {
        go_match(EQL_TOKEN);
    } else {
        if (!is_match(BECOMES_TOKEN) && !is_match(EQL_TOKEN)) {
            print_err(SYNTAX_ERR, NULL);
            print_line_info();
            g_line_err=1;
            return;
        }
    }

    // 变量，常数或表达式
    get_next_token();
    double a = parse_expr();
    if (g_line_err != 1)
        printf("VALUE: %lf\n", a);
}

void const_decl() {
    go_match(IDENT_TOKEN);
    if (match_type() != NUL_TOKEN) {
        go_match(EQL_TOKEN);
    } else {
        if (!is_match(BECOMES_TOKEN) && !is_match(EQL_TOKEN)) {
            print_err(SYNTAX_ERR, NULL);
            print_line_info();
            g_line_err=1;
            return;
        }
    }

    // 常数或表达式
    get_next_token();
    double a = parse_expr();
    if (g_line_err!=1)
        printf("VALUE: %lf\n", a);
}

TokenKind match_type() {
    get_next_token();
    switch (g_cur_tk.kind) {
        case T_INT:
        case T_double:
        case T_STR:
        case T_STRUCT:
            return g_cur_tk.kind;
        case EQL_TOKEN:
        case BECOMES_TOKEN:
            return NUL_TOKEN;
        default:
            return NUL_TOKEN;
    }
}

double parse_expr() {
    double lval = parse_term();
    return parse_expr_tail(lval);
}

double parse_term() {
    double lval = parse_factor();
    return parse_term_tail(lval);
}

double parse_expr_tail(double lval) {
    if (is_match(ADD_OP_TOKEN)) {
        get_next_token();
        double value = lval + parse_term();
        return parse_term_tail(value);
    } else if (is_match(SUB_OP_TOKEN)) {
        get_next_token();
        double value = lval - parse_term();
        return parse_term_tail(value);
    } else if (is_match(END_LINE_TOKEN)) {
        return lval;
    }
    g_line_err = 1;
    print_err(LACK_OF_OP, NULL);
    print_line_info();
}

double parse_term_tail(double lval) {
    if (is_match(MUL_OP_TOKEN)) {
        get_next_token();
        double val = lval * parse_factor();
        return parse_term_tail(val);
    } else if (is_match(DIV_OP_TOKEN)) {
        get_next_token();
        double val = lval / parse_factor();
        return parse_term_tail(val);
    } else {
        return lval;
    }
}

double parse_factor() {
    //print_tk();
    double val = 0;
    // (expr)
    if (is_match(LPAREN_TOKEN)) {
        get_next_token();
        val = parse_expr();
        must_match(RPAREN_TOKEN);
        get_next_token();
    }
    // 常数
    else if (is_match(NUMBER_TOKEN)) {
        val = g_cur_tk.value;
        get_next_token();
        return val;
    }
    // 变量
    // TODO: look up in symbol table
    else if (is_match(IDENT_TOKEN)) {

    }
    else if (is_match(RPAREN_TOKEN)) {
        g_line_err = 1;
        print_err(LACK_OF_PARENT, NULL);
        print_line_info();
    }
    else if (is_cal_ops(g_cur_tk.kind)) {
        g_line_err = 1;
        print_err(TOO_MUCH_OPS, NULL);
        print_line_info();
    }
    return val;
}
