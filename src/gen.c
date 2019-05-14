//
// Created by Tecker on 2019-05-10.
//

#include "../header/gen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

QUATERNION *pQuad = NULL;
int genLineNum = 1;
const char * tmpVarName = "_T";
int genTmpVarID = 0;
char tmpBuf[30];

void gen_init() {
    pQuad = (QUATERNION *)malloc(500 * sizeof(QUATERNION));
}

void gen_destroy() {
    free(pQuad);
}

void print_all_quad() {
    for(int i=1;i<genLineNum;++i) {
        if (strcmp(pQuad[i].op, "goto")!=0)
            printf("%d (%s, %s, %s, %s)\n", i, pQuad[i].op, pQuad[i].l_arg, pQuad[i].r_arg, pQuad[i].result);
        else
            printf("%d goto %s\n", i, pQuad[i].result);
    }
}

void new_Tmp_var() {
    memset(tmpBuf, '\0', sizeof(tmpBuf));
    sprintf(tmpBuf, "%s%d", tmpVarName, genTmpVarID++);
}

char* genCode(char* op, char* left, char* right) {
    new_Tmp_var();
    printf("%d (%s, %s, %s, %s)\n", genLineNum, op, left, right, tmpBuf);

    strcpy(pQuad[genLineNum].op, op);
    strcpy(pQuad[genLineNum].l_arg, left);
    strcpy(pQuad[genLineNum].r_arg, right);
    strcpy(pQuad[genLineNum].result, tmpBuf);

    ++genLineNum;
    return tmpBuf;
}

void genWithoutTmp(char* op, char* left, char* right) {
    printf("%d (%s, %s, %s)\n", genLineNum, op, left, right);

    strcpy(pQuad[genLineNum].op, op);
    strcpy(pQuad[genLineNum].l_arg, left);
    strcpy(pQuad[genLineNum].r_arg, right);
    strcpy(pQuad[genLineNum].result, "_");
    ++genLineNum;
}

void genCondi(char* op, char* left, char* right, int label) {
    printf("%d (%s, %s, %s, L%d)\n", genLineNum, op, left, right, label);

    strcpy(pQuad[genLineNum].op, op);
    strcpy(pQuad[genLineNum].l_arg, left);
    strcpy(pQuad[genLineNum].r_arg, right);
    sprintf(pQuad[genLineNum].result, "L%d", label);

    ++genLineNum;
}

int genCondiPlaceHold(char* op, char* left, char* right, int label) {
    printf("%d (%s, %s, %s, _%d_\n", genLineNum, op, left, right, label);

    strcpy(pQuad[genLineNum].op, op);
    strcpy(pQuad[genLineNum].l_arg, left);
    strcpy(pQuad[genLineNum].r_arg, right);
    sprintf(pQuad[genLineNum].result, "%d", label);

    ++genLineNum;
    return genLineNum-1;
}

void gen_goto_seg(char* p) {
    printf("%d goto %s\n", genLineNum, p);

    strcpy(pQuad[genLineNum].op, "goto");
    strcpy(pQuad[genLineNum].result, p);

    ++genLineNum;
}

int gen_goto_line(int num, int is_placehold) {
    if (is_placehold == PLACEHOLDER)
        printf("%d goto _%d_\n", genLineNum, num);
    else
        printf("%d goto %d\n", genLineNum, num);

    strcpy(pQuad[genLineNum].op, "goto");
    if (is_placehold == PLACEHOLDER)
        sprintf(pQuad[genLineNum].result, "%d", num);
    else
        sprintf(pQuad[genLineNum].result, "L%d", num);

    ++genLineNum;
    return genLineNum-1;
}

int getNextCodeLine() {
    return genLineNum;
}

void back_patch(int pre_line, int label) {
    while(strcmp(pQuad[pre_line].result, "0")!=0) {
        int cur_line = pre_line;
        pre_line = atoi(pQuad[pre_line].result);
        printf("Back patch line %d\n", cur_line);
        memset(pQuad[cur_line].result, '\0', sizeof(pQuad[cur_line].result));
        sprintf(pQuad[cur_line].result, "L%d", label);
    }

    memset(pQuad[pre_line].result, '\0', sizeof(pQuad[pre_line].result));
    sprintf(pQuad[pre_line].result, "L%d", label);
}
