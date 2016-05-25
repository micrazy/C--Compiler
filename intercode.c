#include"intercode.h"
#include <stdlib.h>
#include<stdio.h>
#include<string.h>

InterCode head=NULL;
InterCode tail=NULL;

void insertCode(InterCode c){
    c->prev=NULL;
    c->next=NULL;
    if(head==NULL){
        head=c;
        tail=c;
    }
    else{
        c->prev=tail;
        tail->next=c;
        tail=c;    
    }
}

void deleteCode(InterCode c){
    if(c==NULL||head==NULL||tail==NULL){
        return;
    }
    if(head==c&&tail==c){
        head=NULL;
        tail=NULL;
    }
    else if(head==c){
        head->next->prev=NULL;
        head=c->next;
    }
    else if(tail==c){
        tail->prev->next=NULL;
        tail=c->prev;
    }
    else{
        if(c->next!=NULL)
            c->next->prev=c->prev;
        if(c->prev!=NULL)
            c->prev->next=c->next;
    }
}

void printCode(char * path){
    FILE *fp=fopen(path,"w");
    if(fp==NULL){
        printf("Open file failure!\n");
        return;
    }
    InterCode p=head;
    while(p!=NULL){
        switch(p->kind){
            case ASSIGN:
                printOp(p->u.assign.left,fp);
                fputs(":=  ",fp);
                printOp(p->u.assign.right,fp);
                break;
            case ADD:case SUB:case MUL:case DIV: 
                printOp(p->u.binop.result,fp);
                fputs(":=  ",fp);
                printOp(p->u.binop.op1,fp);
                switch(p->kind){
                    case ADD:
                    fputs("+  ",fp);
                    break;
                    case SUB:
                    fputs("-  ",fp);
                    break;
                    case MUL:
                    fputs("*  ",fp);
                    break;
                    case DIV:
                    fputs("/  ",fp);
                    break;
                }
                printOp(p->u.binop.op2,fp);
                break;
            case RETURN:
                fputs("RETURN  ",fp);
                printOp(p->u.oneop.op,fp);
                fputs(":  ",fp);
                break;
            case LABEL:
                fputs("LABEL  ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case LABEL_GOTO:
                fputs("GOTO  ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case IF_GOTO:
                fputs("IF  ",fp);
                printOp(p->u.ifop.op1,fp);
                fputs(p->u.ifop.op,fp);
                fputs(" ",fp);
                printOp(p->u.ifop.op2,fp);
                fputs("GOTO  ",fp);
                printOp(p->u.ifop.op3,fp);
                break;
            case READ:
                fputs("READ  ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case WRITE:
                fputs("WRITE  ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case CALLFUNC:
                printOp(p->u.assign.left,fp);
                fputs(":=  CALL  ",fp);
                printOp(p->u.assign.right,fp);
                break;
            case FUNCTION:
                fputs("FUNCTION  ",fp);
                printOp(p->u.oneop.op,fp);
                fputs(":  ",fp);
                break;
            case ARG:
                fputs("ARG  ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case PARAM:
                fputs("PARAM  ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case REFASSIGN:
                printOp(p->u.assign.left,fp);
                fputs(":=  &",fp);
                printOp(p->u.assign.right,fp);
                break;
            case DEC:
                fputs("DEC  ",fp);
                printOp(p->u.dec.op,fp);
                char size[32];
                sprintf(size,"%d",p->u.dec.size);
                fputs(size,fp);
                break;
        }
        fputs("\n",fp);
        p=p->next;
    }
    fclose(fp);
}


void printOp(Operand op,FILE *fp){
    if(op==NULL){
        printf("NULL op here\n");
        fputs("t0  ",fp);
        return;
    }
    char tmp[32];
    memset(tmp,0,sizeof(tmp));

    switch(op->kind){
        case VARIABLE:case FUNC:
            fputs(op->u.value,fp);
            break;
        case CONSTANT:
            fputs("#",fp);
            fputs(op->u.value,fp);
            break;
        case ADDRESS:
            fputs("*",fp);
            fputs(op->u.name->u.value,fp);
            break;
    }
    fputs("  ",fp);
}







