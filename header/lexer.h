#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#define MAX_TOKEN_SIZE 100
#define N_KEYWORD 20
#define TRUE 1

char word[N_KEYWORD][MAX_TOKEN_SIZE+1];

typedef enum {
    BAD_TOKEN,      // UNFINISH
    NUL_TOKEN,      // UNKNOWN
    NUMBER_TOKEN,   // 123 , 123.456
    STR_TOKEN,      // "abc" (abc as string value)
    IDENT_TOKEN,    // abc123
    ADD_OP_TOKEN,   // +
    SUB_OP_TOKEN,   // -
    MUL_OP_TOKEN,   // *
    DIV_OP_TOKEN,   // /
    PERCENT_OP_TOKEN, // %
    BIT_AND_OP_TOKEN, // &
    BIT_OR_OP_TOKEN,  // |
    NOT_OP_TOKEN,   // !
    END_LINE_TOKEN, // \n
    BECOMES_TOKEN,  // :=
    EQL_TOKEN,      // =

    // 注释 COMMENT_TOKEN #
    // 界符
    SEMICOLON_TOKEN,        // ;
    COLON_TOKEN,            // :
    COMMA_TOKEN,            // ,
    LPAREN_TOKEN,           // (
    RPAREN_TOKEN,           // )
    LBRACE_TOKEN,           // {
    RBRACE_TOKEN,           // }
    DOT_TOKEN,              // .
    LBRACKET_TOKEN,         // [
    RBRACKET_TOKEN,         // ]
    SINGLE_QUOTE_TOKEN,     // '
    DOUBLE_QUOTE_TOKEN,     // "

    // 关系运算符
    EQ_OP_TOKEN,      // ==
    NEQ_OP_TOKEN,     // !=
    LT_OP_TOKEN,      // <
    LTE_OP_TOKEN,     // <=
    GT_OP_TOKEN,      // >
    GTE_OP_TOKEN,     // >=
    AND_OP_TOKEN,     // &&
    OR_OP_TOKEN,      // ||

    EMPTY_SYM,       // 仅用于相对定位保留字 36

    // 保留字
    T_BOOL,           // bool -- T
    BK_SYM,           // break
    CASE_SYM,         // case
    CONST_SYM,        // const
    CONTI_SYM,        // continue
    DEF_SYM,          // default
    T_double,         // double -- T
    ELIF_SYM,         // elif
    ELSE_SYM,         // else
    FALSE_VAL,        // false
    FOR_SYM,          // for
    FUNC_SYM,         // func
    IF_SYM,           // if
    T_INT,            // int -- T
    RET_SYM,          // ret
    T_STR,            // string -- T
    T_STRUCT,         // struct -- T
    SWITCH_SYM,       // switch
    TRUE_VAL,         // true
    VAR_SYM,          // var

} TokenKind;

char token_literal[EMPTY_SYM][MAX_TOKEN_SIZE+1];

typedef struct {
    TokenKind kind;
    double value;
    char str[MAX_TOKEN_SIZE+1];
} Token;

void lexer_init();
void reset_line(char *line);
void get_token(Token *tk);
int get_line_pos();
int find_word(char *str);
char* t_to_string(TokenKind k);
int IsEOF();

#endif