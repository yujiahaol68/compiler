#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../header/lexer.h"
#include "../header/logger.h"
#include "../header/errno.h"
#include "../header/global.h"
#include "string.h"

int g_line_err;
int g_line_num;

static char *st_line;
static int st_line_pos;
static int end_of_file;
static int in_str;

typedef enum {
    INITIAL_STATUS,
    IN_INT_PART_STATUS,
    IN_FRAC_PART_STATUS,
} LexerStatus;

int get_line_pos() {
    return st_line_pos;
}

void get_token(Token* tk) {
    LexerStatus status = INITIAL_STATUS;
    char cur_ch;
    char msg[50];

    tk->kind = BAD_TOKEN;
    while(st_line[st_line_pos]!='\0') {
        cur_ch = st_line[st_line_pos];

        //printf("%c\n", cur_ch);

        switch (status)
        {
            case IN_INT_PART_STATUS:
                if (cur_ch == '.') {
                    char tmp[2] = {cur_ch, '\0'};
                    strcat(tk->str, tmp);
                    ++st_line_pos;
                    
                    int prev_len = strlen(tk->str);
                    sscanf(&st_line[st_line_pos], "%[0-9]", &(tk->str)[prev_len]);
                    int n_len = strlen(tk->str);

                    if (n_len <= prev_len) {
                        sprintf(msg, "Not a number !! \nIn Line: %d\n", g_line_num);
                        print_err(SYNTAX_ERR, msg);
                        exit(1);
                    }

                    st_line_pos += n_len - prev_len;
                }

                sscanf(tk->str, "%lf", &tk->value);
                tk->kind = NUMBER_TOKEN;
                return;
            case IN_FRAC_PART_STATUS:
                if (isdigit(cur_ch)) {
                    int prev_len = strlen(tk->str);
                    sscanf(&st_line[st_line_pos], "%[0-9]", &(tk->str)[prev_len]);
                    int n_len = strlen(tk->str);

                    if (n_len <= prev_len) {
                        sprintf(msg, "Not a number !! \nIn Line: %d\n", g_line_num);
                        print_err(SYNTAX_ERR, msg);
                        exit(1);
                    }

                    st_line_pos += n_len - prev_len;
                    sscanf(tk->str, "%lf", &tk->value);
                    tk->kind = NUMBER_TOKEN;
                    return;
                }
                sprintf(msg, "Fail get Fraction part in number\nIn Line: %d\n", g_line_num);
                print_err(SYNTAX_ERR, msg);
                exit(1);
            default:
                break;
        }

        if (in_str == 1) {
            sscanf(&st_line[st_line_pos], "%[^\"]", tk->str);
            tk->kind = STR_TOKEN;
            int n_len = strlen(tk->str);
            st_line_pos += n_len;
            if (st_line[st_line_pos] != '"') {
                sprintf(msg, "In Line: %d\nNot a valid string, lack of quote mark", g_line_num);
                print_err(SYNTAX_ERR, msg);
                exit(1);
            }
            in_str = 2;
            return;
        }


        // 忽略: 空格 | 换行 | 回车 | 制表符
        if (cur_ch == ' ' || cur_ch == '\n' || cur_ch == 13 || cur_ch == 9) {
            if (cur_ch == '\n') {
                tk->kind = END_LINE_TOKEN;
                return;
            }
            ++st_line_pos;
            continue;
        }

        // 匹配开头

        // 标识符 | 保留字 (字母)
        if (isalpha(cur_ch)) {
            sscanf(&st_line[st_line_pos], "%[0-9a-z]", tk->str);

            int i = find_word(tk->str);
            if (i==-1)
                tk->kind = IDENT_TOKEN;
            else
                tk->kind = EMPTY_SYM + i;
            st_line_pos += strlen(tk->str);
            return;
        }

        // 整数值 | 浮点值 (数字)
        if (isdigit(cur_ch)) {
            if (status == INITIAL_STATUS) {
                status = IN_INT_PART_STATUS;
                sscanf(&st_line[st_line_pos], "%[0-9]", tk->str);
                st_line_pos += strlen(tk->str);
            }
            continue;
        } else if (cur_ch == '.') {
            // 浮点数 .33
            if (status == INITIAL_STATUS && isdigit(st_line[st_line_pos+1])) {
                status = IN_FRAC_PART_STATUS;
                tk->str[0]='0';
                tk->str[1]='.';
                tk->str[2]='\0';
            } else {
                // 普通 .
                tk->str[0] = cur_ch;
                tk->str[1] = '\0';
                tk->kind = DOT_TOKEN;
                ++st_line_pos;
                return;
            }
            ++st_line_pos;
            continue;            
        }

        //printf("HERE!!");
        tk->str[0] = cur_ch;
        tk->str[1] = '\0';
        // 运算符号 | 注释符号 (其他符号)
        switch (cur_ch)
        {
            // 赋值
            case ':':
                if (st_line[st_line_pos+1]=='=') {
                    tk->str[1] = '=';
                    tk->str[2] = '\0';
                    tk->kind = BECOMES_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = COLON_TOKEN;
                break;
            // 注释
            case '#':
                tk->kind = END_LINE_TOKEN;
                return;
            // 运算符
            case '+':
                tk->kind = ADD_OP_TOKEN;
                break;
            case '-':
                tk->kind = SUB_OP_TOKEN;
                break;
            case '*':
                tk->kind = MUL_OP_TOKEN;
                break;
            case '/':
                tk->kind = DIV_OP_TOKEN;
                break;
            case '%':
                tk->kind = PERCENT_OP_TOKEN;
                break;
            case '&':
                if (st_line[st_line_pos+1]=='&') {
                    tk->str[1] = '&';
                    tk->str[2] = '\0';
                    tk->kind = AND_OP_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = BIT_AND_OP_TOKEN;
                break;
            case '|':
                // 条件 || 或位运算 |
                if (st_line[st_line_pos+1]=='|') {
                    tk->str[1] = '|';
                    tk->str[2] = '\0';
                    tk->kind = OR_OP_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = BIT_OR_OP_TOKEN;
                break;
            // condition
            case '=':
                // 条件或赋值 ==, =
                if (st_line[st_line_pos+1]=='=') {
                    tk->str[1] = '=';
                    tk->str[2] = '\0';
                    tk->kind = EQ_OP_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = EQL_TOKEN; // =
                break;
            case '!':
                // 条件 != 或非运算 !
                if (st_line[st_line_pos+1]=='=') {
                    tk->str[1] = '=';
                    tk->str[2] = '\0';
                    tk->kind = NEQ_OP_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = NOT_OP_TOKEN;
                break;
            // 关系运算符
            case '>':
                if (st_line[st_line_pos+1]=='=') {
                    tk->str[1] = '=';
                    tk->str[2] = '\0';
                    tk->kind = GTE_OP_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = GT_OP_TOKEN;
                break;
            case '<':
                if (st_line[st_line_pos+1]=='=') {
                    tk->str[1] = '=';
                    tk->str[2] = '\0';
                    tk->kind = LTE_OP_TOKEN;
                    st_line_pos+=2;
                    return;
                }
                tk->kind = LT_OP_TOKEN;
                break;
            // 界符
            case '\'':
                tk->kind = SINGLE_QUOTE_TOKEN;break;
            case '"':
                tk->kind = DOUBLE_QUOTE_TOKEN;
                if (in_str == 0) {
                    in_str = 1;
                } else if (in_str == 2) {
                    in_str = 0;
                }
                break;
            case ';':
                tk->kind = SEMICOLON_TOKEN;break;
            case ',':
                tk->kind = COMMA_TOKEN;break;
            case '{':
                tk->kind = LBRACE_TOKEN;break;
            case '}':
                tk->kind = RBRACE_TOKEN;break;
            case '(':
                tk->kind = LPAREN_TOKEN;break;
            case ')':
                tk->kind = RPAREN_TOKEN;break;
            case '[':
                tk->kind = LBRACKET_TOKEN;break;
            case ']':
                tk->kind = RBRACKET_TOKEN;break;
            default:
                tk->kind = NUL_TOKEN;
        }

        ++st_line_pos;
        if (tk->kind == NUL_TOKEN) {
            sprintf(msg, "In Line: %d:%d\n", g_line_num, st_line_pos);
            print_err(SYNTAX_ERR, msg);
            exit(1);
        } else {
            return;
        }
    }
    // out of line
    tk->kind = END_LINE_TOKEN;
    end_of_file = TRUE;
}

void reset_line(char *line) {
    st_line = line;
    st_line_pos = 0;
    end_of_file = 0;
    in_str = 0; // 假定字符串在本行没有被截断
}

void lexer_init() {
    strcpy(&word[0][0],  "bool");
    strcpy(&word[1][0],  "break");
    strcpy(&word[2][0],  "case");
    strcpy(&word[3][0],  "const");
    strcpy(&word[4][0],  "continue");
    strcpy(&word[5][0],  "default");
    strcpy(&word[6][0],  "double");
    strcpy(&word[7][0],  "elif");
    strcpy(&word[8][0],  "else");
    strcpy(&word[9][0],  "false");
    strcpy(&word[10][0], "for");
    strcpy(&word[11][0], "func");
    strcpy(&word[12][0], "if");
    strcpy(&word[13][0], "int");
    strcpy(&word[14][0], "ret");
    strcpy(&word[15][0], "string");
    strcpy(&word[16][0], "struct");
    strcpy(&word[17][0], "switch");
    strcpy(&word[18][0], "true");
    strcpy(&word[19][0], "var");

    strcpy(&token_literal[0][0], "unfinished");
    strcpy(&token_literal[1][0], "unknown");
    strcpy(&token_literal[2][0], "number");
    strcpy(&token_literal[3][0], "string");
    strcpy(&token_literal[4][0], "identifier");
    strcpy(&token_literal[5][0], "+");
    strcpy(&token_literal[6][0], "-");
    strcpy(&token_literal[7][0], "*");
    strcpy(&token_literal[8][0], "/");
    strcpy(&token_literal[9][0], "%%");
    strcpy(&token_literal[10][0], "&");
    strcpy(&token_literal[11][0], "|");
    strcpy(&token_literal[12][0], "!");
    strcpy(&token_literal[13][0], "\\n");
    strcpy(&token_literal[14][0], ":=");
    strcpy(&token_literal[15][0], "=");
    strcpy(&token_literal[16][0], ";");
    strcpy(&token_literal[17][0], ":");
    strcpy(&token_literal[18][0], ",");
    strcpy(&token_literal[19][0], "(");
    strcpy(&token_literal[20][0], ")");
    strcpy(&token_literal[21][0], "{");
    strcpy(&token_literal[22][0], "}");
    strcpy(&token_literal[23][0], ".");
    strcpy(&token_literal[24][0], "[");
    strcpy(&token_literal[25][0], "]");
    strcpy(&token_literal[26][0], "\'");
    strcpy(&token_literal[27][0], "\"");
    strcpy(&token_literal[28][0], "==");
    strcpy(&token_literal[29][0], "!=");
    strcpy(&token_literal[30][0], "<");
    strcpy(&token_literal[31][0], "<=");
    strcpy(&token_literal[32][0], ">");
    strcpy(&token_literal[33][0], ">=");
    strcpy(&token_literal[34][0], "&&");
    strcpy(&token_literal[35][0], "||");
}

// 返回相对空保留字的偏移量，没找到返回 -1
int find_word(char *str) {
    //printf("cmp [%s] [%s]", str, word);
    int lo=0;
    int hi=N_KEYWORD-1;
    while(lo <= hi) {
        int mid = lo+((hi-lo)>>1);
        int t = strcmp(str, word[mid]);
        if (t > 0) {
            lo = mid+1;
        } else if (t < 0) {
            hi = mid-1;
        } else {
            return mid+1;
        }
    }
//    for(int i=0;i<N_KEYWORD;++i) {
//        if (strcmp(str, word[i])==0) return i+1;
//    }
    return -1;
}

char* t_to_string(TokenKind k) {
    if (k > EMPTY_SYM) {
        k = k-EMPTY_SYM-1;
        return word[k];
    }
    return token_literal[k];
}

int IsEOF() {
    return end_of_file;
}
