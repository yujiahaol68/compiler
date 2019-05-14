//
// Created by Tecker on 2019-04-19.
//

#include "../header/parser.h"
#include "../header/logger.h"
#include "../header/global.h"
#include "../header/gen.h"
#include "../header/hashtable.h"
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

int is_condi_ops(TokenKind k) {
    return k >= EQ_OP_TOKEN && k<= OR_OP_TOKEN;
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

Cause parse() {
    while(1) {
        get_next_token();

        if (g_cur_tk.kind == END_LINE_TOKEN) {
            ++g_line_num;
            get_next_token();
            return END_OF_LINE;
        }

        char name[101];
        switch (g_cur_tk.kind) {
            // var a int = 15
            // var b int = 1+2
            // var c int = (b - a) * a / b
            case VAR_SYM:
                var_decl();
                break;
            case IDENT_TOKEN:
                strcpy(name, g_cur_tk.str);
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
                    var_assign(name);
                }
                break;
            // const a int = 1    
            case CONST_SYM:
                const_decl();
                break;
            // if statement    
            case IF_SYM:
                condition_stmt(NULL);
                break;
            // for statement ( while like )    
            case FOR_SYM: {
                int true_out = getNextCodeLine();
                condition_stmt(&true_out);
                break;
            }
            case RBRACE_TOKEN:
                get_next_token();
                return END_OF_BLOCK;
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
            return SYNTAX_ERROR;
        }
    }
}

void var_assign(char* var_name) {
    get_next_token();
    struct Element e = parse_expr();
    if (g_line_err != 1) {
        struct Sym* s = symtable_get_by_name(var_name);
        if (s == NULL) {
            char buf[50];
            sprintf(buf, "Variable %s undefined\n", var_name);
            print_err(SYNTAX_ERR, buf);
            print_line_info();
            g_line_err=1;
            return;
        }
        else if (s->t != e.t) {
            g_line_err = 1;
            print_err(UNMATCH_TYPES, NULL);
            print_line_info();
            return;
        }
        else if (s->kind == NUMBER_TOKEN) {
            char buf[50];
            sprintf(buf, "Can not reassign value to constant %s\n", var_name);
            print_err(SYNTAX_ERR, buf);
            print_line_info();
            g_line_err=1;
            return;
        }

        if (e.k == NUMBER_TOKEN) {
            char c[10];
            sprintf(c, "%.2lf", e.number);
            genWithoutTmp(t_to_string(EQL_TOKEN), var_name, c);
        }
        else if (e.k == IDENT_TOKEN) {
            genWithoutTmp(t_to_string(EQL_TOKEN), var_name, e.name);
        }
    }
}

void var_decl() {
    go_match(IDENT_TOKEN);
    char var_name[101];
    strcpy(var_name, g_cur_tk.str);
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
    struct Element e = parse_expr();
    if (g_line_err != 1) {
        //printf("VALUE: %lf\n", e.number);
        // 变量
        ht_err res = symtable_reg_Var(var_name, NUMBER_TYPE, &e.number, IDENT_TOKEN);
        // 检查重复定义
        if (res == DUP_KEY) {
            char buf[50];
            sprintf(buf, "Variable %s redeclare\n", var_name);
            print_err(SYNTAX_ERR, buf);
            print_line_info();
            g_line_err=1;
            return;
        }

        print_table_key();

        if (e.k == NUMBER_TOKEN) {
            char c[10];
            sprintf(c, "%.2lf", e.number);
            genWithoutTmp(t_to_string(BECOMES_TOKEN), var_name, c);
        }
        // 表达式中有变量
        else if (e.k == IDENT_TOKEN) {
            genWithoutTmp(t_to_string(BECOMES_TOKEN), var_name, e.name);
        }
    }
}

void const_decl() {
    go_match(IDENT_TOKEN);
    char var_name[101];
    strcpy(var_name, g_cur_tk.str);
    TokenKind var_type = match_type();
    if (var_type != NUL_TOKEN) {
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
    if (var_type == T_INT || var_type == T_double) {
        get_next_token();
        struct Element e = parse_expr();
        if (g_line_err!=1) {
            //printf("VALUE: %lf\n", e.number);
            if (e.k != NUMBER_TOKEN && e.k != STR_TOKEN) {
                char buf[50];
                sprintf(buf, "%s should be constant\n", e.name);
                print_err(SYNTAX_ERR, buf);
                print_line_info();
                g_line_err=1;
                return;
            }
            //DataType t = (var_type == T_INT || var_type == T_double) ? NUMBER_TYPE : STRING_TYPE;
            ht_err res = symtable_reg_Var(var_name, NUMBER_TYPE, &e.number, NUMBER_TOKEN);
            // 检查重复定义
            if (res == DUP_KEY) {
                char buf[50];
                sprintf(buf, "Variable %s redeclare\n", var_name);
                print_err(SYNTAX_ERR, buf);
                print_line_info();
                g_line_err=1;
                return;
            } else {
                char c[10];
                sprintf(c, "%.2lf", e.number);
                genWithoutTmp(t_to_string(BECOMES_TOKEN), var_name, c);
                print_table_key();
            }
        }
    } else if (var_type == T_STR) {
        get_next_token();
        must_match(DOUBLE_QUOTE_TOKEN);
        get_next_token();
        must_match(STR_TOKEN);

        char c[101];
        strcpy(c, g_cur_tk.str);

        get_next_token();
        must_match(DOUBLE_QUOTE_TOKEN);

        if (g_line_err!=1) {
            ht_err res = symtable_reg_Var(var_name, STRING_TYPE, c, STR_TOKEN);
            if (res == DUP_KEY) {
                char buf[50];
                sprintf(buf, "Variable %s redeclare\n", var_name);
                print_err(SYNTAX_ERR, buf);
                print_line_info();
                g_line_err=1;
                return;
            } else {
                genWithoutTmp(":", var_name, c);
                print_table_key();
            }
        }
    }
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

struct Element parse_expr() {
    struct Element first = parse_term();
    return parse_expr_tail(&first);
}

struct Element parse_term() {
    struct Element first = parse_factor();
    return parse_term_tail(&first);
}

struct Element parse_expr_tail(struct Element* first) {
    struct Element second;
    // +T, -T
    if (is_match(ADD_OP_TOKEN) || is_match(SUB_OP_TOKEN)) {
        TokenKind op = g_cur_tk.kind;
        get_next_token();
        second = parse_term();
        genByElement(op, first, &second);
        return parse_expr_tail(first);
    }
    // END
    //else if (is_match(END_LINE_TOKEN)) {
    //    return *first;
    //}
    //g_line_err = 1;
    //print_err(LACK_OF_OP, NULL);
    //print_line_info();
    return *first;
}

struct Element parse_term_tail(struct Element* first) {
    // *F, /F
    if (is_match(MUL_OP_TOKEN) || is_match(DIV_OP_TOKEN)) {
        TokenKind op = g_cur_tk.kind;
        get_next_token();
        struct Element second = parse_factor();
        genByElement(op, first, &second);
        return parse_term_tail(first);
    }
    // END
    else {
        return *first;
    }
}

struct Element parse_factor() {
    //print_tk();
    struct Element e;
    // (E)
    if (is_match(LPAREN_TOKEN)) {
        get_next_token();
        e = parse_expr();
        must_match(RPAREN_TOKEN);
        get_next_token();
    }
    // 常数
    else if (is_match(NUMBER_TOKEN)) {
        e = get_Elem(g_cur_tk);
        get_next_token();
        return e;
    }
    // 变量
    else if (is_match(IDENT_TOKEN)) {
        e = get_Elem(g_cur_tk);
        get_next_token();
        return e;
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
    return e;
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
        struct Sym* s = symtable_get_by_name(g_cur_tk.str);
        //printf("need to find %s\n", g_cur_tk.str);
        if (s == NULL) {
            char buf[50];
            sprintf(buf, "Variable %s undefined\n", g_cur_tk.str);
            print_err(SYNTAX_ERR, buf);
            print_line_info();
            g_line_err=1;
        }
        else if (s->t != NUMBER_TYPE) {
            char buf[100];
            sprintf(buf, "Expect number, but variable %s is not a number\n", g_cur_tk.str);
            print_err(SYNTAX_ERR, buf);
            print_line_info();
            g_line_err=1;
        }
        // 是常量
        else if (s->kind == NUMBER_TOKEN) {
            e.t = s->t;
            e.k = s->kind;
            double* val = (double*)s->val;
            e.number = *val;
        }
        // 是变量
        else if (s->kind == IDENT_TOKEN) {
            e.t = s->t;
            e.k = s->kind;
        }
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
    if (g_line_err == 1) return;
    if (first->t != second->t) {
        //printf("...%d...%d\n", first->t, second->t);
        //printf("..%s..%s..%s", first->name, t_to_string(k), second->name);
        g_line_err = 1;
        print_err(UNMATCH_TYPES, NULL);
        print_line_info();
        return;
    }

    if (first->t == second->t == NUMBER_TYPE) {
        // 都是常数（排除常量）
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
                    break;
                default:
                    printf("Invalid OP: %s", t_to_string(k));
                    return;
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

struct Element parse_bool_expr(int* pre_t, int* pre_f) {
    get_next_token();
    struct Element l_args = parse_expr();
    int pre_true = 0;
    int pre_false = 0;
    while(g_cur_tk.kind!=LBRACE_TOKEN) {
        TokenKind condi_op = g_cur_tk.kind;
        // ==, !=, >, >=, <, <=
        if (condi_op >= EQ_OP_TOKEN && condi_op <= GTE_OP_TOKEN) {
            get_next_token();
            struct Element r_args = parse_expr();
            if ((l_args.k == IDENT_TOKEN || l_args.k == NUMBER_TOKEN)
            && (r_args.k == IDENT_TOKEN || l_args.k == NUMBER_TOKEN)) {
                pre_true = genCondiPlaceHold(t_to_string(condi_op), l_args.name, r_args.name, pre_true);
                pre_false = gen_goto_line(pre_false, PLACEHOLDER);
            } else {
                printf("Invalid boolean expr!!\n");
            }
        }
        // &&
        else if (condi_op==AND_OP_TOKEN) {

        }
        // ||
        else if (condi_op==OR_OP_TOKEN) {

        }
    }

    *pre_t = pre_true;
    *pre_f = pre_false;
    return l_args;
}

void condition_stmt(const int* true_out) {
    int pre_true = 0;
    int pre_false = 0;
    parse_bool_expr(&pre_true, &pre_false);
    // {
    must_match(LBRACE_TOKEN);
    get_next_token();
    must_match(END_LINE_TOKEN);
    ++g_line_num;

    back_patch(pre_true, getNextCodeLine());

    printf("................\n");
    char buf[1024];
    while(fgets(buf, 1024, stdin) != NULL) {
        reset_line(buf);
        Cause c = parse();
        if (c == END_OF_BLOCK) break;
    }

    if (true_out != NULL) {
        gen_goto_line(*true_out, NORMAL);
    }
    // } else {
    if (g_cur_tk.kind == ELSE_SYM) {
        // {\n
        get_next_token();
        must_match(LBRACE_TOKEN);
        get_next_token();
        must_match(END_LINE_TOKEN);
        ++g_line_num;

        pre_true = gen_goto_line(0, PLACEHOLDER);
        back_patch(pre_false, getNextCodeLine());

        while(fgets(buf, 1024, stdin) != NULL) {
            reset_line(buf);
            Cause c = parse();
            if (c == END_OF_BLOCK) break;
        }

        must_match(END_LINE_TOKEN);
        back_patch(pre_true, getNextCodeLine());
    }
    // }\n
    else if (g_cur_tk.kind == END_LINE_TOKEN) {
        printf("..........\n");
        back_patch(pre_false, getNextCodeLine());
    }
}
