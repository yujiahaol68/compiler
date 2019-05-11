//
// Created by Tecker on 2019-04-19.
//

#include "../header/parser.h"
#include "../header/logger.h"
#include "../header/global.h"
#include "../header/gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // +T
    if (is_match(ADD_OP_TOKEN)) {
        get_next_token();
        double value = lval + parse_term();
        return parse_term_tail(value);
    }
    // -T
    else if (is_match(SUB_OP_TOKEN)) {
        get_next_token();
        double value = lval - parse_term();
        return parse_term_tail(value);
    }
    // END
    else if (is_match(END_LINE_TOKEN)) {
        return lval;
    }
    g_line_err = 1;
    print_err(LACK_OF_OP, NULL);
    print_line_info();
}

double parse_term_tail(double lval) {
    // *F
    if (is_match(MUL_OP_TOKEN)) {
        get_next_token();
        double val = lval * parse_factor();
        return parse_term_tail(val);
    }
    // /F
    else if (is_match(DIV_OP_TOKEN)) {
        get_next_token();
        double val = lval / parse_factor();
        return parse_term_tail(val);
    }
    // END
    else {
        return lval;
    }
}

double parse_factor() {
    //print_tk();
    double val = 0;
    // (E)
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

struct Element get_Elem(Token t) {
    struct Element e;
    e.k = t.kind;
    // 常数
    if (t.kind == NUMBER_TOKEN) {
        e.t = NUMBER_TYPE;
        e.number = t.value;
    }
    // 字符串
    else if (t.kind == STR_TOKEN) {
        e.t = STRING_TYPE;
    }
    // 变量
    else if (t.kind == IDENT_TOKEN) {
        // TODO: 查符号表得到变量的类型和值
    }
    // 布尔值
    else if (t.kind == TRUE_VAL) {
        e.t = BOOL_TYPE;
        e.number = 1;
    } else if (t.kind == FALSE_VAL) {
        e.t = BOOL_TYPE;
        e.number = 0;
    }
    strcpy(e.name, t.str);
    return e;
}

void genByElement(TokenKind k, struct Element* first, struct Element* second) {
    if (first->t != second->t) {
        g_line_err = 1;
        print_err(UNMATCH_TYPES, NULL);
        print_line_info();
        return;
    }

    // 生成赋值代码
    if (k == BECOMES_TOKEN) {
        genWithoutTmp(t_to_string(k), first->name, second->name);
        return;
    }

    if (first->t == second->t == NUMBER_TYPE) {
        // 都是常量
        if (first->k == NUMBER_TOKEN && second->k == NUMBER_TOKEN) {
            switch (k) {
                case ADD_OP_TOKEN:
                    first->number += second->number;
                    break;
                case SUB_OP_TOKEN:
                    first->number -= second->number;
                    break;
                case MUL_OP_TOKEN:
                    first->number *= second->number;
                    break;
                case DIV_OP_TOKEN:
                    first->number /= second->number;
            }
        }
        // 其中任意一个是变量
        else {
            // 左操作数成为新的临时变量
            char* op = t_to_string(k);
            char* tmpName = genCode(op, first->name, second->name);
            memset(first->name, '\0', sizeof(first->name));
            strcpy(first->name, tmpName);
            first->k = IDENT_TOKEN;
        }
    }
    else if (first->t == second->t == STRING_TYPE) {
        if (first->k == STR_TOKEN && second->k == STR_TOKEN) {
            // 直接合并字符串
            if (strlen(first->name)+strlen(second->name) < 101) {
                strcat(first->name, second->name);
                return;
            }
        }
        // 其中一个是变量或生成代码合并字符串
        char* tmpName = genCode("++", first->name, second->name);
        memset(first->name, '\0', sizeof(first->name));
        strcpy(first->name, tmpName);
        first->k = IDENT_TOKEN;
    }
}
