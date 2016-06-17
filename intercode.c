#include"reg.h"
#include<stdio.h>
#include<string.h>
#define REGNUM 26
InterCode head=NULL;
InterCode tail=NULL;

reg_t reg[REGNUM];
var_t *varlist=NULL;
int offset=8;//8=4 (ra) +   4 (fp)

int varcount=1;
int labcount=1;

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
                break;
            case LABEL:
                fputs("LABEL  ",fp);
                printOp(p->u.oneop.op,fp);
                fputs("  :",fp);
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
       // printf("NULL op here\n");
        fputs("t0  ",fp);
        return;
    }
    char tmp[32];
    memset(tmp,0,sizeof(tmp));

    switch(op->kind){
        case VARIABLE:
            fputs(op->u.value,fp);
            scanVar(op->u.value);
            break;
        case FUNC:
            fputs(op->u.value,fp);
            break;
        case TEMPVAR:
            fputs("t",fp);
            sprintf(tmp,"%d",op->u.var_no);
            fputs(tmp,fp);
            printf("aa");
            scanVar(tmp);
            break;
        case CONSTANT:
            fputs("#",fp);
            fputs(op->u.value,fp);
            break;
        case LABEL:
            fputs("label",fp);
            sprintf(tmp,"%d",op->u.var_no);
            fputs(tmp,fp);
            break;
        case VADDRESS:
           // printf("aaaaaaaaa");
            fputs("*",fp);
            fputs(op->u.name->u.value,fp);
            break;
        case TADDRESS:
            fputs("*t",fp);
            sprintf(tmp,"%d",op->u.name->u.var_no,fp);
            fputs(tmp,fp);
            break;
    }
    fputs("  ",fp);
}



void printObjCode(char *path){
    FILE *fp=fopen(path,"w");
    if(fp==NULL){
        printf("Open file failure!\n");
        return;
    }
    


    fputs(".data\n_prompt: .asciiz \"Enter an integer:\"\n",fp);
    fputs("_ret: .asciiz \"\\n\"\n.globl main\n.text\n",fp);
    fputs("read:\n",fp);
    fputs("li $v0, 4\nla $a0, _prompt\n",fp);
    fputs("syscall\nli $v0, 5\nsyscall\njr $ra\n",fp);

    fputs("\nwrite:\n",fp);
    fputs("li $v0, 1\nsyscall\nli $v0, 4\n",fp);
    fputs("la $a0, _ret\nsyscall\nmove $v0, $0\njr $ra\n",fp);
    
    InterCode p=head;
    while(p!=NULL){


        int r1=-1,r2=-1,r3=-1;
        char *name1=NULL,*name2=NULL,*name3=NULL;
        Operand right=NULL,left=NULL;
        Operand result=NULL,op1=NULL,op2=NULL,op3=NULL;
        char *op=NULL;
        switch(p->kind){
            case ASSIGN:
                left=p->u.assign.left;
                right=p->u.assign.right;
                if(left->kind==TEMPVAR||left->kind==VARIABLE){
                    name1=genname(left);
       //             name2=genname(right);
                    switch(right->kind){
                        case CONSTANT:
                            r1=allocReg();
                            regtoVar(name1,r1);
                            fputs("li ",fp);
                            printReg(r1,fp);
                            fputs(" , ",fp);
                            fputs(right->u.value,fp);
                        break;
                        case VARIABLE:case TEMPVAR:
                            r1=allocReg();
                            regtoVar(name1,r1);
                                
                            name2=genname(right);
                            r2=allocReg();
                            regtoVar(name2,r2);

                            vartoreg(name2,r2,fp);
                            fputs("move ",fp);
                            printReg(r1,fp);
                            fputs(" , ",fp);
                            printReg(r2,fp);
                        break;
                    
                    
                    }
                
                }
                break;
            case ADD:
            case SUB:
            case MUL:
            case DIV:    
                result=p->u.binop.result;
                op1=p->u.binop.op1;
                op2=p->u.binop.op2;
                switch(result->kind){
                    case TEMPVAR:case VARIABLE:
                        name1=genname(result);
                        if(op1->kind==TEMPVAR||op1->kind==VARIABLE){
                            name2=genname(op1);
                            if(op2->kind==TEMPVAR||op2->kind==VARIABLE){
                                name3=genname(op2);
                                r1=allocReg();
                                r2=allocReg();
                                regtoVar(name2,r2);
                                r3=allocReg();
                                regtoVar(name3,r3);
                                
                                regtoVar(name1,r1);

                                vartoreg(name2,r2,fp);
                                vartoreg(name3,r3,fp);

                                if(p->kind==ADD){
                                    fputs("add ",fp);
                                }
                                else if(p->kind==SUB)
                                    fputs("sub ",fp);
                                else if(p->kind==MUL)
                                    fputs("mul ",fp);
                                else 
                                    fputs("div ",fp);
                                printReg(r1,fp);
                                fputs(" , ",fp);
                                printReg(r2,fp);
                                fputs(" , ",fp);
                                printReg(r3,fp);

                            }
                            else
                            if(op2->kind==CONSTANT){
                                r1=allocReg();
                                r2=allocReg();
                                regtoVar(name2,r2);
                                    
                                regtoVar(name1,r1);
                                vartoreg(name2,r2,fp);

                                if(p->kind==ADD||p->kind==SUB){
                                    fputs("addi ",fp);
                                    printReg(r1,fp);
                                    fputs(" , ",fp);
                                    printReg(r2,fp);
                                    fputs(" , ",fp);
                                    if(p->kind==SUB){
                                        fputs("-",fp);
                                    }
                                    fputs(op2->u.value,fp);
                                }
                                else 
                                if(p->kind==MUL){
                                    r3=allocReg();
                                    fputs("li ",fp);
                                    printReg(r3,fp);
                                    fputs(" , ",fp);
                                    fputs(result->u.value,fp);
                                    fputs("\n",fp);
                                    fputs("mul ",fp);
                                    printReg(r1,fp);
                                    fputs(" , ",fp);
                                    printReg(r2,fp);
                                    fputs(" , ",fp);
                                    printReg(r3,fp);
                                }
                                else if(p->kind==DIV){
                                    r3=allocReg();
                                    fputs("li ",fp);
                                    printReg(r3,fp);
                                    fputs(" , ",fp);
                                    fputs(result->u.value,fp);
                                    fputs("\n",fp);
                                    fputs("div ",fp);
                                    printReg(r2,fp);
                                    fputs(" , ",fp);
                                    printReg(r3,fp);
                                    fputs("\nmflo ",fp);
                                    printReg(r1,fp);        
                                
                                }
                            
                            }
                        
                        }
                    break;

                
                
                }
                break;
            case RETURN:
                fputs("subu $sp, $fp, 8\n",fp);
                fputs("lw $fp, 0($sp)\n",fp);
                fputs("addi $sp, $sp, 4\n",fp);
                fputs("lw $ra, 0($sp)\n",fp);
                fputs("addi $sp, $sp, 4\n",fp);
                
                if(p->u.oneop.op->kind==CONSTANT){   
                    r1=allocReg();
                    fputs("li ",fp);
                    printReg(r1,fp);
                    fputs(" ",fp);
                    fputs(p->u.oneop.op->u.value,fp);
                    fputs("\n",fp);
                    fputs("move $v0, ",fp);
                    printReg(r1,fp);
                    fputs("\njr $ra",fp);
                    break;

                }
                r1=allocReg();
                char *name1=genname(p->u.oneop.op);
                 
                vartoreg(name1,r1,fp);
                regtoVar(name1,r1);
                fputs("move $v0, ",fp);
                printReg(r1,fp);
                fputs("\njr $ra",fp);
                break;
            case LABEL:
                printOp(p->u.oneop.op,fp);
                fputs(":\n",fp);
                break;
            case LABEL_GOTO:
                fputs("j ",fp);
                printOp(p->u.oneop.op,fp);
                break;
            case IF_GOTO:
                op1=p->u.ifop.op1;
                op2=p->u.ifop.op2;
                op3=p->u.ifop.op3;
                op=p->u.ifop.op;
                
                name1=genname(op1);
                name2=genname(op2);
                

                r1=allocReg();
                r2=allocReg();
                
                regtoVar(name2,r2);
                regtoVar(name1,r1);
                vartoreg(name1,r1,fp);
                vartoreg(name2,r2,fp);

                if(strcmp(op,"==")==0)
                    fputs("beq",fp);
                else if(strcmp(op,"!=")==0)
                    fputs("bne",fp);
                else if(strcmp(op,">")==0)
                    fputs("bgt",fp);
                else if(strcmp(op,"<")==0)
                    fputs("blt",fp);
                else if(strcmp(op,">=")==0)
                    fputs("bge",fp);
                else if(strcmp(op,"<=")==0)
                    fputs("ble",fp);
                
                fputs(" ",fp);
                printReg(r1,fp),
                fputs(" , ",fp);
                printReg(r2,fp);
                fputs(" , ",fp);
                printOp(op3,fp);
                break;
            case READ:
                fputs("addi $sp, $sp, -4\n",fp);
                fputs("sw $ra, 0($sp)\n",fp);
                
                fputs("jal read\n",fp);
                fputs("lw $ra, 0($sp)\n",fp);
                fputs("addi $sp, $sp, 4\n",fp);
                r1=allocReg();
                name1=genname(p->u.oneop.op);
                regtoVar(name1,r1);
                fputs("move ",fp);
                printReg(r1,fp);
                fputs(" , $v0",fp);
                break;
            case WRITE:
                r1=allocReg();
                name1=genname(p->u.oneop.op);
                
                vartoreg(name1,r1,fp);
                regtoVar(name1,r1);

                fputs("move $a0 , ",fp);
                printReg(r1,fp);
                fputs("\n",fp);

                fputs("addi $sp, $sp, -4\n",fp);
                fputs("sw $ra, 0($sp)\n",fp);
                fputs("jal write\n",fp);
                fputs("lw $ra, 0($sp)\n",fp);
                fputs("addi $sp, $sp, 4\n",fp);

                break;
            case CALLFUNC:
                break;
            case FUNCTION:
                fputs("\n",fp);
                printOp(p->u.oneop.op,fp);
                fputs(":\n",fp);
                fputs("subu $sp, $sp, 4\n",fp);
                fputs("sw $ra, 0($sp)\n",fp);//store $ra
                fputs("subu $sp, $sp, 4\n",fp);
                fputs("sw $fp, 0($sp)\n",fp);//store $fp
                fputs("addi $fp, $sp, 8\n",fp);//compute fp
                
                fputs("subu $sp, $sp, 1024\n",fp); //stack size 1024
                
                if(p->next==NULL)return ;//no params
                break;
            case ARG:
                break;
            case PARAM:
                break;
            case REFASSIGN:
                break;
            case DEC:
                break;
        }
        fputs("\n",fp);
        //push reg to stack
        var_t *v=varlist;
        while(v!=NULL){ 
            if(v->reg>=0){
                fputs("subu $v1 ,$fp , ",fp);
                char temp[32];
                memset(temp,0,32);
                sprintf(temp,"%d",v->offset);
                fputs(temp,fp);
                    

                fputs("\nsw ",fp);
                printReg(v->reg,fp);
                fputs(", 0($v1)\n",fp);
                reg[v->reg].used=0;
                v->reg=-1;
            }
            v=v->next;
        }
        fputs("\n",fp);


   p=p->next;
    }
}

int allocReg(){
    int i=0;
    for(i;i<REGNUM;i++){
        if(reg[i].used==0){
            reg[i].used=1;
            return i;
        }
    
    }

    return -1;


}

void printReg(int index,FILE *fp){
    fputs("$",fp);
    fputs(reg[index].name,fp);
}



void initReg(){
    int i;
    for(i=0;i<REGNUM;i++){
        reg[i].used=0;    
    }
    reg[0].name[0]='t';
    reg[0].name[1]='0';
    reg[1].name[0]='t';
    reg[1].name[1]='1';
    reg[2].name[0]='t';
    reg[2].name[1]='2';
    reg[3].name[0]='t';
    reg[3].name[1]='3';
    reg[4].name[0]='t';
    reg[4].name[1]='4';
    reg[5].name[0]='t';
    reg[5].name[1]='5';
    reg[6].name[0]='t';
    reg[6].name[1]='6';
    reg[7].name[0]='t';
    reg[7].name[1]='7';
    reg[8].name[0]='t';
    reg[8].name[1]='8';
    reg[9].name[0]='t';
    reg[9].name[1]='9';

    reg[10].name[0]='s';
    reg[10].name[1]='0';
    reg[11].name[0]='s';
    reg[11].name[1]='1';
    reg[12].name[0]='s';
    reg[12].name[1]='2';
    reg[13].name[0]='s';
    reg[13].name[1]='3';
    reg[14].name[0]='s';
    reg[14].name[1]='4';
    reg[15].name[0]='s';
    reg[15].name[1]='5';
    reg[16].name[0]='s';
    reg[16].name[1]='6';
    reg[17].name[0]='s';
    reg[17].name[1]='7';

    reg[18].name[0]='f';
    reg[18].name[1]='p';

    reg[19].name[0]='s';
    reg[19].name[1]='p';

    reg[20].name[0]='r';
    reg[20].name[1]='a';

    reg[21].name[0]='v';
    reg[21].name[1]='0';

    reg[22].name[0]='a';
    reg[22].name[1]='0';
    reg[23].name[0]='a';
    reg[23].name[1]='1';
    reg[24].name[0]='a';
    reg[24].name[1]='2';
    reg[25].name[0]='a';
    reg[25].name[1]='3';



    //init varlist

}


void scanVar(char *name){//scan all var
    if(varlist!=NULL){
        var_t *tmp=varlist;
        while(tmp!=NULL){
            if(strcmp(name,tmp->name)==0)
                return;
            tmp=tmp->next;
        }
    }
    var_t *var=(var_t *)malloc(sizeof(var_t));
    var->name=name;
    var->reg=-1;
    offset=offset+4;
    var->offset=offset;
    if(varlist==NULL){
        varlist=var;
        varlist->next=NULL;
    }
    else{
        var_t *tmp=varlist;
        while(tmp->next!=NULL){
            tmp=tmp->next;
        }
        tmp->next=var;
    }

}

char *genname(Operand p){
    char *varname=(char *)malloc(sizeof(16));
    if(p->kind==TEMPVAR){
        sprintf(varname,"t%d",p->u.var_no);
    }
    else 
    if(p->kind==VARIABLE){
        strcpy(varname,p->u.value);
    }
    else{
        fprintf(stderr,"wrong kind\n");
        return NULL;
    }

    return varname;
}

void vartoreg(char *name,int index,FILE *fp){
    //load var from stack to reg
    var_t* v=varlist;
    while(v!=NULL){
        if(strcmp(v->name,name)==0){
            break;
        }
        v=v->next;
    }
    if(v==NULL)
        fprintf(stderr,"var not exists in stack\n");
    fputs("lw ",fp);
    printReg(index,fp);
    char tmp[16];
    sprintf(tmp,", %d($fp)\n",-v->offset);
    fputs(tmp,fp);

}

void regtoVar(char *name,int index){//set var.reg
    var_t *v=varlist;
    while(v!=NULL){
        if(strcmp(v->name,name)==0){
            v->reg=index;
        
        }
        v=v->next;
    }

}
