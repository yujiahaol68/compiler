//
// Created by Tecker on 2019-05-10.
//

#include "../header/gen.h"
#include <stdio.h>
#include <string.h>

int genLineNum = 1;
const char * tmpVarName = "_T";
int genTmpVarID = 0;
char tmpBuf[30];

void new_Tmp_var() {
    memset(tmpBuf, '\0', sizeof(tmpBuf));
    sprintf(tmpBuf, "%s%d", tmpVarName, genTmpVarID++);
}

char* genCode(char* op, char* left, char* right) {
    new_Tmp_var();
    printf("%d (%s, %s, %s, %s)\n", genLineNum++, op, left, right, tmpBuf);
    return tmpBuf;
}

char* genWithoutTmp(char* op, char* left, char* right) {
    printf("%d (%s, %s, %s)\n", genLineNum++, op, left, right);
}