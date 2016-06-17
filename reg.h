#ifndef __REG_H__
#define __REG_H__
#include"intercode.h"
#include <stdlib.h>

typedef struct reg_t_{
    char name[2];
    int used;
}reg_t;

void initReg();
void printReg(int index,FILE *fp);
int allocReg();
char *genname(Operand p);
void regtoVar(char *name,int index);
void vartoreg(char *name,int index,FILE *fp);
void scanVar(char *name);

typedef struct var_t_{
    char* name;
    int reg;
    int offset;
    struct var_t_ * next;
}var_t;

extern var_t *varlist;
#endif
