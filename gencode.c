#include "gencode.h"

Operand newtemp(){
    Operand op=(Operand)malloc(sizeof(struct Operand_));
    op->kind=TEMPVAR;
    op->u.var_no=varcount++;
    return op;
}

Operand newlabel(){
    Operand op=(Operand)malloc(sizeof(struct Operand_));
    op->kind=LAB;
    op->u.var_no=labcount++;
    return op;
}

Operand newvar(char *name){
    Operand op=(Operand)malloc(sizeof(struct Operand_));
    op->kind=VARIABLE;
    op->u.value=name;
    return op;
}

int typeSize(Type t){
    if(t->kind==0){
        return 4;//int only
    }
    else if(t->kind==1){//array
        return t->u.array.size*typeSize(t->u.array.elem); 
    }
    else{
    //no struct 
    
    }


}



/*************start gencode***********************/
void  dd(struct typeNode *root){
    tProgram(root);
}


void tProgram(struct typeNode * root){
    if(root==NULL)return; 
    tExtDefList(root->childNode);
}

void tExtDefList(struct typeNode* node){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    if(p!=NULL){
        tExtDef(p);
        tExtDefList(p->brotherNode);
    }
}


void tExtDef(struct typeNode* node){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode; 
    p=p->brotherNode;
    if(strcmp(p->type,"ExtDecList")==0){
        //Specifier ExtDecList SEMI
        //no global var
        return;

       // ExtDecList(p,q);
    }
    else if(strcmp(p->type,"SEMI")==0){
        //Specifier SEMI
        return ;
    }
    else{
        //Specifier FunDec CompSt
        Functype f=find_fun(p->childNode->text);
        if(f==NULL){
            printf("func not exists\n");
            return;
        }
        
        Operand funop=(Operand)malloc(sizeof(struct Operand_));
        funop->kind=FUNC;
        funop->u.value=f->name;
        InterCode c=(InterCode)malloc(sizeof(struct InterCode_));
        c->kind=FUNCTION;
        c->u.oneop.op=funop;
        
        insertCode(c);
        FieldList param=f->param;
        while(param!=NULL){
            Operand op=newvar(param->name);
            InterCode cc=(InterCode)malloc(sizeof(struct InterCode_));
            cc->kind=PARAM;
            cc->u.oneop.op=op;
            insertCode(cc);
            param=param->next;    
        }
        tCompSt(p->brotherNode);
        }
            
}


void tVarDec(struct typeNode* node,int from){//1:variable 2:struct 3:parma
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
    if(strcmp(p->type,"ID")==0){
        FieldList f=find_var(p->text);
        if(f==NULL){
            printf("variable not exists @ %d\n",__LINE__);
        }
        if(from==1&&f->type->kind==2){
            //struct should print dec
            printf("i can't trans structure! @%d\n",__LINE__);
        }
        return;
    } 
    else{
        tVarDec(p,from);
        //VarDec LB INT RB
        //size of array
        return ;
    }

}  



void tCompSt(struct typeNode *node){
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
    p=p->brotherNode;
    if(strcmp(p->type,"DefList")==0){
        tDefList(p,1);
        tStmtList(p->brotherNode);
    }
    else
        tStmtList(p);
}

void tStmtList(struct typeNode *node){
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
    if(p==NULL)return;
    tStmt(p);

    tStmtList(p->brotherNode);
}

void tStmt(struct typeNode *node){
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
       // printf("%s",p->type);
    if(strcmp(p->type,"Exp")==0){
        //Exp SEMI
        tExp(p,NULL);
        return;
    }
    else if(strcmp(p->type,"CompSt")==0){
        //CompSt
        tCompSt(p);
        return;
    }
    else if(strcmp(p->type,"RETURN")==0){
        //RETURN Exp SEMI
        p=p->brotherNode;
        Operand op=newtemp();
        tExp(p,op);
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=RETURN;
        code->u.oneop.op=op;
        insertCode(code);
        return;
    }
    else if(strcmp(p->type,"IF")==0){
        //IF LP Exp RP Stmt (ELSE Stmt)
        p=p->brotherNode->brotherNode;
        tStmt(p);
            //..
    }
    else if(strcmp(p->type,"WHILE")==0){
        //WHILE LP Exp RP Stmt
        p=p->brotherNode->brotherNode;
        p=p->brotherNode->brotherNode;
        tStmt(p);
    }
}


void tExp(struct typeNode *node,Operand place){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    if(strcmp(p->type,"Exp")==0){
        struct typeNode *p1=p->brotherNode;
        if(strcmp(p1->type,"ASSIGNOP")==0){
            //Exp ASSIGNOP Exp
            Operand left=newtemp();
            struct typeNode *q=p->childNode;
            if(strcmp(q->type,"ID")==0&&q->brotherNode==NULL){//variable
                                                              
                tExp(p,left);
            }
            else if(strcmp(q->type,"Exp")==0&&q->brotherNode!=NULL&&strcmp(q->brotherNode->type,"LB")==0){
                
                tExp(p,left);
            }
            else if(strcmp(q->type,"Exp")==0&&q->brotherNode!=NULL&&strcmp(q->brotherNode->type,"DOT")==0){
                //STRUCt
                printf("i cann't trans structure!@%d\n",__LINE__);
            }
            
            Operand right=newtemp();
            int temp_no=right->u.var_no;//for optim

            tExp(p1->brotherNode,right);
            
            if(right->kind==TEMPVAR&&right->u.var_no==temp_no&&(left->kind=TEMPVAR||left->kind==VARIABLE)){
                memcpy(right,left,sizeof(struct Operand_));
            }
            else{
                InterCode code1=malloc(sizeof(struct InterCode_));
                code1->kind=ASSIGN;
                code1->u.assign.left=left;
                code1->u.assign.right=right;
                insertCode(code1);
            }
            InterCode code2=malloc(sizeof(struct InterCode_));
            code2->kind=ASSIGN;
            code2->u.assign.left=place;
            code2->u.assign.right=right;
            if(place!=NULL)
                insertCode(code2);
        }
        else if(strcmp(p1->type,"MUL")==0||strcmp(p1->type,"DIV")==0||\
                strcmp(p1->type,"ADD")==0||strcmp(p1->type,"SUB")==0){
                    //type be same
                    Operand op1=newtemp();

                    tExp(p,op1);

                    Operand op2=newtemp();
                    

                    tExp(p1->brotherNode,op2);
                    
                    InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
                    code->u.binop.op1=op1;
                    code->u.binop.op2=op2;
                    code->u.binop.result=place;
                    if(strcmp(p1->type,"ADD")==0){
                        code->kind=ADD;
                    }
                    else if(strcmp(p1->type,"SUB")==0)
                        code->kind=SUB;
                    else if(strcmp(p1->type,"MUL")==0)
                        code->kind=MUL;
                    else if(strcmp(p1->type,"DIV")==0)
                        code->kind=DIV;
                    if(place!=NULL)
                        insertCode(code);
        }
        else if(strcmp(p1->type,"OR")==0||strcmp(p1->type,"AND")==0||strcmp(p1->type,"RELOP")==0){
                    Operand op1=newlabel();
                    Operand op2=newlabel();
                    
                    if(place!=NULL){
                        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
                        code->kind=ASSIGN;
                        code->u.assign.left=place;
                        
                        Operand c0=(Operand)malloc(sizeof(struct Operand_));
                        c0->kind=CONSTANT;
                        c0->u.value=(char *)malloc(sizeof(32));
                        strcpy(c0->u.value,"0");
                        code->u.assign.right=c0;
                        insertCode(code);//code0
                    }
                    tExp_Cond(node,op1,op2);//code1
                    
                    InterCode lb1code=(InterCode)malloc(sizeof(struct InterCode_));
                    lb1code->kind=LABEL;
                    lb1code->u.oneop.op=op1;
                    insertCode(lb1code);// label1
                    
                    Operand c1=(Operand)malloc(sizeof(struct Operand_));
                    c1->kind=CONSTANT;
                    c1->u.value=(char *)malloc(sizeof(32));
                    strcpy(c1->u.value,"1");

                    InterCode code2=(InterCode)malloc(sizeof(struct InterCode_));//code2
                    code2->kind=ASSIGN;
                    code2->u.assign.left=place;
                    code2->u.assign.right=c1;
                    if(place!=NULL)
                       insertCode(code2); 

                    InterCode lb2code=(InterCode)malloc(sizeof(struct InterCode_));
                    lb2code->kind=LABEL;
                    lb2code->u.oneop.op=op2;
                    insertCode(lb2code);

        }
        else if(strcmp(p1->type,"LB")==0){
            //array
            Operand op1=newtemp();
            
            tExp(p,op1);
            FieldList f1=find_var(p->text);
            if(f1==NULL)
                printf("VARIABLE not exists@%d",__LINE__);
            //FieldList f2=find_var(p1->brotherNode->text);
            Operand op2=newtemp();

            tExp(p1->brotherNode,op2);
            
            Operand offset=newtemp();

            Operand intsize=(Operand)malloc(sizeof(struct Operand_));
            intsize->kind=CONSTANT;
            intsize->u.value=(char  *)malloc(32);
            memset(intsize->u.value,0,32);
            sprintf(intsize->u.value,"%d",typeSize(f1->type->u.array.elem));
            InterCode addrcode=(InterCode)malloc(sizeof(struct InterCode_));
            addrcode->kind=MUL;
            addrcode->u.binop.result=offset;
            addrcode->u.binop.op1=op2;
            addrcode->u.binop.op2=intsize;
            insertCode(addrcode);
    
            Operand temp=newtemp();
            InterCode code1=(InterCode)malloc(sizeof(struct InterCode_));
            code1->kind=ADD;
            if(f1->type->kind==0){
                place->kind=TADDRESS;
                place->u.name=temp;
                code1->u.binop.result=temp;
            }
            else{
                code1->u.binop.result=place;
            }
            code1->u.binop.op1=op1;
            code1->u.binop.op2=offset;
            insertCode(code1);
        }
        else if(strcmp(p1->type,"DOT")==0){
            printf("i can't trans structure@%d\n",__LINE__);
        }    
    }
    else if(strcmp(p->type,"LP")==0){
           tExp(p->brotherNode,place); 
    }
    else if(strcmp(p->type,"SUB")==0){
        Operand op=newtemp();

        tExp(p->brotherNode,op);
        
        Operand op2=(Operand)malloc(sizeof(struct Operand_));
        op2->kind=CONSTANT;
        op2->u.value=(char *)malloc(4);
        strcpy(op2->u.value,"0");
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=SUB;
        code->u.binop.result=place;
        code->u.binop.op1=op2;
        code->u.binop.op2=op;
        if(place!=NULL)
            insertCode(code);
        return ;
        
    }
    else if(strcmp(p->type,"NOT")==0){
        Operand op1=newlabel();
        Operand op2=newlabel();
        if(place!=NULL){
            InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
            code->kind=ASSIGN;
            code->u.assign.left=place;
            Operand c0=(Operand)malloc(sizeof(struct Operand_));
            c0->kind=CONSTANT;
            c0->u.value=(char *)malloc(sizeof(32));
            strcpy(c0->u.value,"0");
            code->u.assign.right=c0;
            insertCode(code);//code0
        }
        tExp_Cond(node,op1,op2);//code1
        InterCode lb1code=(InterCode)malloc(sizeof(struct InterCode_));
        lb1code->kind=LABEL;
        lb1code->u.oneop.op=op1;
        insertCode(lb1code);// label1

        Operand c1=(Operand)malloc(sizeof(struct Operand_));
        c1->kind=CONSTANT;
        c1->u.value=(char *)malloc(sizeof(32));
        strcpy(c1->u.value,"1");

        InterCode code2=(InterCode)malloc(sizeof(struct InterCode_));//code2
        code2->kind=ASSIGN;
        code2->u.assign.left=place;
        code2->u.assign.right=c1;
        if(place!=NULL)
            insertCode(code2); 

        InterCode lb2code=(InterCode)malloc(sizeof(struct InterCode_));
        lb2code->kind=LABEL;
        lb2code->u.oneop.op=op2;
        insertCode(lb2code);
        return ;
    }
    else if(strcmp(p->type,"ID")==0){
        struct typeNode *q=p->brotherNode;
        if(q==NULL){
            FieldList f=find_var(p->text);
            place->kind=VARIABLE;
            place->u.value=f->name;
        }
        else {
        //ID LP Args RP 
            Functype f=find_fun(p->text);
            if(strcmp(p->brotherNode->brotherNode->text,"RP")==0){
                if(strcmp(f->name,"read")==0){
                InterCode rpcode=(InterCode)malloc(sizeof(struct InterCode_));
                rpcode->kind=READ;
                rpcode->u.oneop.op=place;
                if(place!=NULL)
                    insertCode(rpcode);
            
                }
                else{
                    Operand funop=newvar(f->name);
                    funop->kind=FUNCTION;
                    InterCode cfcode=(InterCode)malloc(sizeof(struct InterCode_));
                    cfcode->kind=CALLFUNC;
                    cfcode->u.assign.left=place;
                    cfcode->u.assign.right=funop;
                    insertCode(cfcode);
                }
            }
            else{
                Operand argsList=(Operand)malloc(sizeof(struct Operand_));
                argsList->next=NULL;
                struct typeNode *pp=q->brotherNode;
                tArgs(pp,argsList);

                if(strcmp(f->name,"write")==0){
                    InterCode wacode=(InterCode)malloc(sizeof(struct InterCode_));
                    wacode->kind=WRITE;
                    wacode->u.oneop.op=argsList->next;
                    insertCode(wacode);
                }
                else{
                    argsList=argsList->next;
                    while(argsList!=NULL){
                        InterCode argcode=(InterCode)malloc(sizeof(struct InterCode_));
                        argcode->kind=ARG;
                        argcode->u.oneop.op=argsList;
                        insertCode(argcode);
                        argsList=argsList->next;
                    }
                    Operand funop=newvar(f->name);
                    funop->kind=FUNCTION;
                    InterCode cfcode=(InterCode)malloc(sizeof(struct InterCode_));
                    cfcode->kind=CALLFUNC;
                    cfcode->u.assign.left=place;
                    cfcode->u.assign.right=funop;
                    insertCode(cfcode);

                
                }
            }
        }
    }
    else if(strcmp(p->type,"INT")==0){
        if(place!=NULL){
            place->kind=CONSTANT;
            place->u.value=p->text;
        }
    }
    else if(strcmp(p->type,"FLOAT")==0){
        printf("not support @%d",__LINE__);
    }
    else{
        printf("UNKOWN TYPE at Exp()\n");
    }
}

void tArgs(struct typeNode *node,Operand args){
    struct typeNode *p=node->childNode;
    Operand t1=newtemp();    
    tExp(p,t1);
    t1->next=args->next;
    args->next=t1;
    if(p->brotherNode!=NULL)
    tArgs(p->brotherNode->brotherNode,args);
    else
    return;
}


void tVarList(struct typeNode* node){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    tParamDec(p);
    
    p=p->brotherNode;
    if(p==NULL){
        //ParamDec
        return ;
    }
    else{
        //ParamDec COMMA VarList
        tVarList(p->brotherNode); 
        return ;
    }

}

void  tParamDec(struct typeNode *node){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    tVarDec(p->brotherNode,3);
    return ;
}


void tDefList(struct typeNode *node,int from){
    if(node==NULL)return ;
    struct typeNode* p=node->childNode;
    if(p==NULL)return;
    tDef(p,from);
    tDefList(p->brotherNode,from);
}

void tDef(struct typeNode *node,int from){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    tDecList(p->brotherNode,from);
}

void tDecList(struct typeNode *node,int from){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    tDec(p,from);
    if(p->brotherNode!=NULL){
        tDecList(p->brotherNode->brotherNode,from);
    }
}

void tDec(struct typeNode *node,int from){
    if(node==NULL)return ;
    struct typeNode* p=node->childNode;
    tVarDec(p,from);
    //variable
    FieldList f=find_var(p->text);
    if(f==NULL){
        printf("var %s not exists! @%d",p->text,__LINE__);
        return;
    }
    if(f->type->kind==1&&from==1){
        //array should dec
        Operand op=newtemp();
        InterCode deccode=(InterCode)malloc(sizeof(struct InterCode_));
        deccode->kind=DEC;
        deccode->u.dec.op=op;
        deccode->u.dec.size=typeSize(f->type);
        insertCode(deccode);

        Operand op1=newvar(f->name);
        InterCode addrcode=(InterCode)malloc(sizeof(struct InterCode_));
        addrcode->kind=REFASSIGN;
        addrcode->u.assign.left=op1;
        addrcode->u.assign.right=op;
        insertCode(addrcode);
    }
    if(p->brotherNode!=NULL){
    //VarDec ASSIGNOP EXP
        Operand place=newvar(f->name);
        p=p->brotherNode;
        tExp(p->brotherNode,place);

        if(place->kind!=VARIABLE||place->u.value!=f->name){
            Operand left=newvar(f->name);
            InterCode asscode=(InterCode)malloc(sizeof(struct InterCode_));
            asscode->kind=ASSIGN;
            asscode->u.assign.left=left;
            asscode->u.assign.right=place;
            insertCode(asscode);
        }
    }
}

void tExp_Cond(struct typeNode * node ,Operand label_true,Operand label_false){
    struct typeNode* p=node->childNode;
    if(strcmp(p->text,"Exp")==0){
        struct typeNode* p1=p->brotherNode;
        if(strcmp(p1->text,"RELOP")==0){
            Operand t1=newtemp();
            Operand t2=newtemp();
            
            tExp(p,t1);//code1
            tExp(p1->brotherNode,t2);//code2

            InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));
            code3->kind=IF_GOTO;
            code3->u.ifop.op1=t1;
            code3->u.ifop.op=p1->text;
            code3->u.ifop.op2=t2;
            code3->u.ifop.op3=label_true;
            insertCode(code3);
            InterCode gotolb=(InterCode)malloc(sizeof(struct InterCode_));
            gotolb->kind=LABEL_GOTO;
            gotolb->u.oneop.op=label_false;
            insertCode(gotolb);
            return ;
        }
        else if(strcmp(p1->text,"AND")==0){
            Operand lb1=newlabel();

            tExp_Cond(p,lb1,label_false);//code1


            InterCode lb1code=(InterCode)malloc(sizeof(struct InterCode_));
            lb1code->kind=LABEL;
            lb1code->u.oneop.op=lb1;
            insertCode(lb1code);
            
            tExp_Cond(p1->brotherNode,label_true,label_false);//code2
            return ;
       
        }
        else if(strcmp(p1->text,"OR")==0){
            Operand lb1=newlabel();

            tExp_Cond(p,label_true,lb1);//code1

            InterCode lb1code=(InterCode)malloc(sizeof(struct InterCode_));
            lb1code->kind=LABEL;
            lb1code->u.oneop.op=lb1;
            insertCode(lb1code);
            
            tExp_Cond(p1->brotherNode,label_true,label_false);//code2
            return ;
        }
    }
    if(strcmp(p->text,"NOT")==0){
        tExp_Cond(p,label_false,label_true);
        return;
    }
    Operand t1=newtemp();
    tExp(node,t1);//code1
    InterCode code2=malloc(sizeof(struct InterCode_));
    code2->kind=IF_GOTO;
    code2->u.ifop.op1=t1;
    code2->u.ifop.op=malloc(2);
    strcpy(code2->u.ifop.op,"!=");
    Operand t2=malloc(sizeof(struct Operand_));
    t2->kind=CONSTANT;
    t2->u.value=malloc(32);
    strcpy(t2->u.value,"0");
    code2->u.ifop.op2=t2;
    code2->u.ifop.op3=label_true;
    insertCode(code2);//code2

    InterCode gotolbf=malloc(sizeof(struct InterCode_));
    gotolbf->kind=LABEL_GOTO;
    gotolbf->u.oneop.op=label_false;
    insertCode(gotolbf);

}


/****************for test*************/




