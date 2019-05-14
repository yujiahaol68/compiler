//
// Created by Tecker on 2019-05-10.
//

#ifndef COMPILER_GEN_H
#define COMPILER_GEN_H
#define MAX_LENGTH 50

#define PLACEHOLDER 1
#define NORMAL 0

typedef struct QUAD{
    char op[8];
    char l_arg[MAX_LENGTH];
    char r_arg[MAX_LENGTH];
    char result[20];
} QUATERNION;

void gen_init();
void gen_destroy();
char* genCode(char* op, char* left, char* right);
void genWithoutTmp(char* op, char* left, char* right);
void genCondi(char* op, char* left, char* right, int label);
int genCondiPlaceHold(char* op, char* left, char* right, int label);
void gen_goto_seg(char* p);
int gen_goto_line(int num, int is_placehold);
int getNextCodeLine();
void back_patch(int pre_line, int label);

void print_all_quad();

#endif //COMPILER_GEN_H
