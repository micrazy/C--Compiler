#ifndef _INTERCODE_H_
#define _INTERCODE_H_
#include<stdio.h>
typedef struct Operand_* Operand;
typedef struct InterCode_ * InterCode;

struct Operand_ {
    enum { VARIABLE=0, CONSTANT, ADDRESS,FUNC} kind;
    union {
        int var_no;//NUM_OF_LABEL 
        char* value;//CONSTANT VARIABLE
        Operand name;//ADDRESS
          } u;
    Operand next;//params
};

struct InterCode_
{
    enum { ASSIGN=0, ADD, SUB, MUL,DIV,RETURN,
          LABEL,LABEL_GOTO,IF_GOTO,
          READ,WRITE,CALLFUNC,FUNCTION,ARG,PARAM,REFASSIGN,DEC
         } kind;
    union {
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;//add sub mul div
        struct { Operand op;} oneop;//return label goto read write 
        struct { Operand op1,op2,op3; char *op; }ifop;//if_goto
        struct { Operand op;int size; }dec;
         } u;
    InterCode prev,next;
};


void insertCode(InterCode c);
void deleteCode(InterCode c);
void printCode(char *path);
void printOp(Operand op,FILE *fp);



extern InterCode head;
extern InterCode tail;





#endif
