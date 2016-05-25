#ifndef _INTERCODE_H_
#define _INTERCODE_H_

typedef struct Operand_* Operand;
typedef struct InterCode_ * InterCode;

struct Operand_ {
    enum { VARIABLE, CONSTANT, ADDRESS} kind;
    union {
        int var_no;
        int value;
       //.. 
          } u;
};

struct InterCode_
{
    enum { ASSIGN, ADD, SUB, MUL,DIV,RETURN,
          LABEL_CODE,LABEL_TRUE,LABEL_GOTO,
          READ,WRITE,CALLFUNC,FUNCTION,ARG,PARAM,REFASSIGH,DEC
         } kind;
    union {
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;//add sub mul div
        struct { Operand op;} oneop;//return label goto read write 
        //…
         } u;
    InterCode prev,next;
};


void insertCode(InterCode c);
void deleteCode(InterCode c);
void printCode(InterCode c);




extern InterCode head;
extern InterCode tail;





#endif
