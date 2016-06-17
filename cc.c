#include "gencode.h"
#include "reg.h"
FieldList vartable[16384];
Functype funtable[16384];

unsigned int hash_pjw(char *name){
    unsigned int val=0,i;
    for(;*name;++name){
        val=(val<<2)+*name;
        if(i=val & ~0x3fff)val=(val ^ (i>>12))&0x3fff;
    }
    return val;
}
void init(){
    //init
    int i=0;
    for(;i<16384;i++){
        vartable[i]=NULL;
        funtable[i]=NULL;
    }


    //add read() write()
    Functype fread=(Functype)malloc(sizeof(struct Functype_));
    Functype fwrite=(Functype)malloc(sizeof(struct Functype_));
    fread->name=(char *)malloc(8);
    strcpy(fread->name,"read");
    fread->line=0;
    Type rtype=(Type)malloc(sizeof(struct Type_));
    rtype->kind=BASIC;
    rtype->u.basic=0;//int
    fread->returnType=rtype;
    fread->param=NULL;
    fread->hashnext=NULL;

    fwrite->name=(char *)malloc(8);
    strcpy(fwrite->name,"write");
    fwrite->line=0;
    fwrite->returnType=rtype;
    fwrite->param=(FieldList)malloc(sizeof(struct FieldList_));
    fwrite->param->name=(char *)malloc(sizeof(16));
    strcpy(fwrite->param->name,"write_param");
    fwrite->param->type=rtype;
    fwrite->param->next=NULL;
    fwrite->param->hashnext=NULL;
    fread->hashnext=NULL;
    
    insertFun(fread);
    insertFun(fwrite);

   /*  for test
    InterCode p=(InterCode)malloc(sizeof(struct InterCode_));
    p->kind=ASSIGN;
    Operand p1=(Operand)malloc(sizeof(struct Operand_));
    Operand p2=(Operand)malloc(sizeof(struct Operand_));
    p1->kind=VARIABLE;
    p1->u.value=(char *)malloc(16);
    strcpy(p1->u.value,"test");
    p1->next=NULL;

    p2->kind=CONSTANT;
    p2->u.value=(char *)malloc(16);
    strcpy(p2->u.value,"16");
    p2->next=NULL;

    p->u.assign.left=p1;
    p->u.assign.right=p2;
    
    insertCode(p);
    printCode("/home/lzc/Desktop/out.ir");


*/ 





}

FieldList find_var(char *name){
    unsigned int hashcode=hash_pjw(name);
    if(vartable[hashcode]==NULL)return NULL;
    else{
        FieldList f=vartable[hashcode];
        while(f!=NULL){
            if(strcmp(name,f->name)==0){
                return f;
            }
            f=f->hashnext;
        }
        return NULL;
    }
}

Functype find_fun(char *name){
    unsigned int hashcode=hash_pjw(name);
    if(funtable[hashcode]==NULL)return NULL;
    else{
        Functype f=funtable[hashcode];
        while(f!=NULL){
            if(strcmp(name,f->name)==0){
                return f;
            }
            f=f->hashnext;
        }
        return NULL;
    }
}

int equalField(FieldList s1,FieldList s2){
    if(s1==NULL&&s2==NULL)return 0;
    if(s1==NULL||s2==NULL)return -1;
    if(isEqual(s1->type,s2->type)==-1)return -1;
    else{
        return equalField(s1->next,s2->next);
    }

}

int isEqual(Type t1,Type t2){
    if(t1==NULL&&t2==NULL)return 0;
    if(t1==NULL||t2==NULL)return -1;
   // fprintf(stderr,"%d %d\n",t1->kind,t2->kind);
    if(t1->kind==t2->kind){
        if(t1->kind==BASIC){
            if(t1->u.basic!=t2->u.basic)return -1;
            else return 0;
        }
        else if(t1->kind==ARRAY){
            return isEqual(t1->u.array.elem,t2->u.array.elem);
        }
        else if(t1->kind==STRUCTURE){
            return equalField(t1->u.structure,t2->u.structure);   
        }
    }
    else 
    return -1;
}


int insertVar(FieldList f){
    if(f==NULL)return -1;
    if(f->name==NULL)return 0;
    //printf("%s",f->name);
    unsigned int hashcode=hash_pjw(f->name);
    if(vartable[hashcode]==NULL){
        //var not defined
        vartable[hashcode]=f;
        return 0;
    }
    else{
        FieldList p=vartable[hashcode];
        while(p!=NULL){
            if(strcmp(p->name,f->name)==0)return 1;//redefine
            if(p->hashnext==NULL){
                p->hashnext=f;
                return 0;
            }
            else
            p=p->hashnext;
        }
        return 0;
    }
}

int insertParam(FieldList f){
    //insert params of function
    FieldList param=f;
    while(param!=NULL){
        int res=insertVar(param);
        if(res==1){
            printf("Error type 3 at line %d:Redefined variable '%s'\n",f->line,f->name);
        }
        param=param->next;
    }

}


int insertFun(Functype f){
    if(f==NULL)return -1;
    if(f->name==NULL)return 0;
    unsigned int hashcode=hash_pjw(f->name);
    if(funtable[hashcode]==NULL){
        //func not defined
        funtable[hashcode]=f;
    }
    else{
        Functype p=funtable[hashcode];
        while(p!=NULL){
            if(strcmp(p->name,f->name)==0)return 1;//redefine
            if(p->hashnext==NULL){
                p->hashnext=f;
                return 0;
            }
            else
            p=p->hashnext;
        }
        return 0;
    }
}

/*************start cc***********************/
void  cc(struct typeNode *root,char *path,char * path1){
    init();
    Program(root);
    //printtable(vartable);
    dd(root);
    printCode(path);
    initReg();
    printObjCode(path1);
}


void Program(struct typeNode * root){
    if(root==NULL)return; 
    ExtDefList(root->childNode);
}

void ExtDefList(struct typeNode* node){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    if(p!=NULL){
        ExtDef(p);
        ExtDefList(p->brotherNode);
    }
}


void ExtDef(struct typeNode* node){
    if(node==NULL)return ;
    struct typeNode *p=node->childNode;
    Type q;
    //analysis the type of q
    q=Specifier(p);
    if(q==NULL)return; 
    p=p->brotherNode;
    if(strcmp(p->type,"ExtDecList")==0){
        //Specifier ExtDecList SEMI
        ExtDecList(p,q);
    }
    else if(strcmp(p->type,"SEMI")==0){
        //Specifier SEMI
        return ;
    }
    else{
        //Specifier FunDec CompSt
        Functype f;
        f=FunDec(p,q);
        int flag=insertFun(f);
        if(flag==1){
            printf("Error type 4 at Line %d:Redefined Function '%s'\n",f->line,f->name);
        }
        else if(flag==-1){
            printf("FuncDef failure\n");
        }
        else{
        CompSt(p->brotherNode,f->returnType);
        }
    }
            
}

Type Specifier(struct typeNode* p){
    if(p==NULL)return NULL;
    struct typeNode* q=p->childNode;
    if(strcmp(q->type,"TYPE")==0){
        //TYPE
        Type t=(Type)malloc(sizeof(struct Type_));
        t->kind=BASIC;
        if(strcmp(q->text,"int")==0){
            t->u.basic=0;//0:int 1:float
        }
        else if(strcmp(q->text,"float")==0){
            t->u.basic=1;
        }
        return t;
    }
    else{
        //STRUCTSpecifier
        Type t;
        t=StructSpecifier(q);
        return t;
    }
}

void ExtDecList(struct typeNode* node,Type t){
    struct typeNode *p=node->childNode;
    if(p->brotherNode==NULL){
        //VarDec
        VarDec(p,t,0);
    }
    else{
        //VarDec COMMA ExtDecList
        VarDec(p,t,0);
        ExtDecList(p->brotherNode->brotherNode,t);
    }
}

FieldList VarDec(struct typeNode* node,Type t,int from){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    if(strcmp(p->type,"ID")==0){
        //ID
        FieldList f=(FieldList)malloc(sizeof(struct FieldList_));
        f->name=p->text;
        f->line=p->line;
        f->type=t;
        f->next=f->hashnext=NULL;
       // printf("%s",f->name);
        //insert into hashtable
        int res=insertVar(f);
        
        if(res==1){
            if(from==0){
            printf("Error type 3 at Line %d: Redefined variable '%s'\n",f->line,f->name);
            free(f);
            return NULL;
            }
            else if(from==1){
            printf("Error type 15 at Line %d: Redefined field '%s'\n",f->line,f->name);
            free(f);
            return NULL;

            }
        }
        return f;

    } 
    else{
        //VarDec LB INT RB
        //size of array
        int size=atoi(p->brotherNode->brotherNode->text);
        Type tt=(Type)malloc(sizeof(struct Type_));
        tt->kind=ARRAY;
        tt->u.array.elem=t;
        tt->u.array.size=size;

        FieldList f=VarDec(p,t,from);
        if(f==NULL){
            free(tt);
            return NULL;
        }
        else{

            if(f->type->kind!=ARRAY){
                f->type=tt;
                return f;
            }
            Type ttt=f->type;
            while(ttt->u.array.elem->kind==ARRAY){
                ttt=ttt->u.array.elem;
            }
            ttt->u.array.elem=tt;
        
        return f;
        }
    }

}

Functype FunDec(struct typeNode *node,Type q){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    //p->type=ID
    char *id=p->text;
    Functype f=(Functype)malloc(sizeof(struct Functype_));
    f->name=id;
    f->line=p->line;
    f->returnType=q;
    f->param=NULL;
    f->hashnext=NULL;
    p=p->brotherNode->brotherNode;
    if(strcmp(p->type,"VarList")==0){
    //ID LP VarList RP
    f->param=VarList(p);
    }
    else{
    //ID LP RP
    f->param=NULL;
    
    }


    return f;
}


void CompSt(struct typeNode *node,Type t){
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
    p=p->brotherNode;
    if(strcmp(p->type,"DefList")==0){
        DefList(p,0);
        StmtList(p->brotherNode,t);
    }
    else
        StmtList(p,t);
}

void StmtList(struct typeNode *node,Type t){
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
    if(p==NULL)return;
    Stmt(p,t);

    StmtList(p->brotherNode,t);
}

void Stmt(struct typeNode *node,Type t){
    if(node==NULL)return;
    struct typeNode *p=node->childNode;
    if(strcmp(p->type,"Exp")==0){
        //Exp SEMI
        Exp(p);
    }
    else if(strcmp(p->type,"CompSt")==0){
        //CompSt
        CompSt(p,t);
    }
    else if(strcmp(p->type,"RETURN")==0){
        //RETURN Exp SEMI

        p=p->brotherNode;
        Type exp_type=Exp(p);
        if(isEqual(exp_type,t)==-1){
            printf("Error type 8 at Line %d:Type mismatched for return\n",p->line);
        }
    }
    else if(strcmp(p->type,"IF")==0){
        //IF LP Exp RP Stmt (ELSE Stmt)
        p=p->brotherNode->brotherNode;
        Type exp_type=Exp(p);
        if(exp_type!=NULL){
        Type tmpType=(Type)malloc(sizeof(struct Type_));
        tmpType->kind=BASIC;
        tmpType->u.basic=0;
        if(isEqual(exp_type,tmpType)==-1){
            printf("Error type 7 at Line %d:Type mismatched\n",p->line);
        }
        free(tmpType);
        }
        p=p->brotherNode->brotherNode;
        Stmt(p,t);
        if(p->brotherNode!=NULL){
            Stmt(p->brotherNode->brotherNode,t);
        }
    }
    else if(strcmp(p->type,"WHILE")==0){
        //WHILE LP Exp RP Stmt
        p=p->brotherNode->brotherNode;
        Type exp_type=Exp(p);
        Type tmpType=(Type)malloc(sizeof(struct Type_));
        tmpType->kind=BASIC;
        tmpType->u.basic=0;
        if(isEqual(exp_type,tmpType)==-1){
            printf("Error type 7 at Line %d:Type mismatched\n",p->line);
        }
        free(tmpType);

        p=p->brotherNode->brotherNode;
        Stmt(p,t);
    }
}


Type Exp(struct typeNode *node){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    //fprintf(stderr,"%s",p->type);
    if(strcmp(p->type,"Exp")==0){
        struct typeNode *p1=p->brotherNode;
        if(strcmp(p1->type,"ASSIGNOP")==0){
            //Exp ASSIGNOP Exp
            //left value
            struct typeNode *q=p->childNode;
            Type leftType=NULL;
            if(strcmp(q->type,"ID")==0&&q->brotherNode==NULL){//variable
                leftType=Exp(p);
            }
            else if(strcmp(q->type,"Exp")==0&&q->brotherNode!=NULL&&strcmp(q->brotherNode->type,"LB")==0){
                //Array
                leftType=Exp(p);
            }
            else if(strcmp(q->type,"Exp")==0&&q->brotherNode!=NULL&&strcmp(q->brotherNode->type,"DOT")==0){
                //STRUCT
                leftType=Exp(p);
            }
            else
                printf("Error type 6 at Line %d: The Left-hand side of an assignment must be a variable\n",p->line);
            
            
            Type rightType=Exp(p1->brotherNode);
            if(leftType==NULL||rightType==NULL){
            return NULL;
            }
            if(isEqual(leftType,rightType)==-1){
                printf("Error type 5 at Line %d:Type mismatched for assignment\n",p->line);
                return NULL;
            }
            else
                return leftType; 
        }
        else if(strcmp(p1->type,"MUL")==0||strcmp(p1->type,"DIV")==0||strcmp(p1->type,"RELOP")==0||\
                strcmp(p1->type,"ADD")==0||strcmp(p1->type,"SUB")==0){
                    //type be same
                    Type t1=Exp(p);
                    Type t2=Exp(p1->brotherNode);
                    if(t1==NULL||t2==NULL)return NULL;
                    if(isEqual(t1,t2)==-1){
                    printf("Error type 7 at Line %d:Type mismatched for operands\n",p->line);
                        return NULL;
                    }
                    else 
                    return t1;
        }
        else if(strcmp(p1->type,"OR")==0||strcmp(p1->type,"AND")==0){
                    Type t1=Exp(p);
                    Type t2=Exp(p1->brotherNode);
                    Type tmpType=(Type)malloc(sizeof(struct Type_));
                    tmpType->kind=BASIC;
                    tmpType->u.basic=0;
                    if(isEqual(t1,tmpType)==-1||isEqual(t2,tmpType)==-1){
                        printf("Error type 7 at Line %d:Type mismatched for operands\n",p->line);
                        free(tmpType);
                        return NULL;
                    }
                    free(tmpType);
                     
                    if(isEqual(t1,t2)==-1){
                    printf("Error type 7 at Line %d:Type mismatched for operands\n",p->line);
                        return NULL;
                    }
                    else 
                    return t1;
        }
        else if(strcmp(p1->type,"LB")==0){
            //array
            Type t1=Exp(p);
            if(t1==NULL)return NULL;
            if(t1->kind!=ARRAY){
                //TODO:make output bertter
                printf("Error type 10 at Line %d:It is not an array\n",p->line);
                return NULL;
            }
            else{
                Type t2=Exp(p1->brotherNode);
                if(t2==NULL)return NULL;
                if(t2->kind!=BASIC||t2->u.basic!=0){
                    //TODO: make output better
                    printf("Error type 12 at Line %d:Array need an integer\n",p->line);
                    return NULL;
                }
                return t1->u.array.elem;
            }
            
        }
        else if(strcmp(p1->type,"DOT")==0){
            Type t1=Exp(p);
            if(t1==NULL)return NULL;
            if(t1->kind!=STRUCTURE){
                printf("Error type 13 at Line %d: Illegal use of '.'\n",p->line);
                return NULL;
            }
            else{
                FieldList f=t1->u.structure;
                while(f!=NULL){
                if(strcmp(f->name,p1->brotherNode->text)==0)
                    return f->type;
                f=f->next;
                }
                printf("Error type 14 at Line %d:Non-existent field '%s'\n",p1->line,p1->brotherNode->text);
                return NULL;
            }
        
        }
    }
    else if(strcmp(p->type,"LP")==0){
        return Exp(p->brotherNode); 
    }
    else if(strcmp(p->type,"SUB")==0){
        Type t=Exp(p->brotherNode);
        if(t==NULL)return NULL;
        if(t->kind!=BASIC){
            printf("Error type 7 at Line %d:Type mismatched for operands\n",p->line);
            return NULL;
        }
        return t;
        
    }
    else if(strcmp(p->type,"NOT")==0){
        Type t=Exp(p->brotherNode);
        if(t==NULL)return NULL;
        if(t->kind!=BASIC||t->u.basic!=0){
            printf("Error type 7 at Line %d:Type mismatched for operands\n",p->line);
            return NULL;
        }
        return t;

    }
    else if(strcmp(p->type,"ID")==0){
        struct typeNode *q=p->brotherNode;
        if(q==NULL){
        //ID
            FieldList f=find_var(p->text);
            if(f==NULL){
                printf("Error type 1 at Line %d:Undefined variable '%s'\n",p->line,p->text);
                return NULL;
            }
            else{
                return f->type;
            }
        }
        else {
        //ID LP Args RP 
            Functype f=find_fun(p->text);
            FieldList f1=find_var(p->text);
            if(f==NULL&&f1==NULL){
                printf("Error type 2 at Line %d: Undefined function '%s'\n",p->line,p->text);
                return NULL;
            }
            if(f==NULL&&f1!=NULL){
                printf("Error type 11 at Line %d: '%s' is not a function\n",p->line,f1->name);
                return NULL;
            }
            FieldList param=f->param;
            struct typeNode *pp=q->brotherNode;
            if(strcmp(pp->type,"Args")==0){
                if(Args(pp,param)==-1){
                    printf("Error type 9 at Line %d: Function '%s(",p->line,p->text);
                    printParam(param);
                    printf(")' is not applicable for arguments '(");
                    printArgs(pp);
                    printf(")'\n");
                    return NULL;
                }
                else
                return f->returnType;
            }
            else{
            if(param!=NULL){
                    printf("Error type 9 at Line %d: Function '%s(",p->line,p->text);
                    printParam(param);
                    printf(")' is not applicable for arguments '()'\n");
                return NULL;
            }
            return f->returnType;
            
            }
        }
    }
    else if(strcmp(p->type,"INT")==0){
        Type tmp=(Type)malloc(sizeof(struct Type_));
        tmp->kind=BASIC;
        tmp->u.basic=0;
        return tmp;
    }
    else if(strcmp(p->type,"FLOAT")==0){
        Type tmp=(Type)malloc(sizeof(struct Type_));
        tmp->kind=BASIC;
        tmp->u.basic=1;
        return tmp;
    }
    else{
        printf("UNKOWN TYPE at Exp()\n");
    }
}

int Args(struct typeNode *node,FieldList param){
    if(node==NULL&&param==NULL)return 0;
    if(node==NULL||param==NULL)return -1;
    struct typeNode *p=node->childNode;
    Type t=Exp(p);
    if(t==NULL)return 0;
    if(isEqual(t,param->type)==-1)return -1;
    if(p->brotherNode==NULL&&param->next==NULL)return 0;
    else if(p->brotherNode==NULL||param->next==NULL)return -1;
    return Args(p->brotherNode->brotherNode,param->next);
}


FieldList VarList(struct typeNode* node){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    FieldList f=NULL;
    f=ParamDec(p);
    
    p=p->brotherNode;
    if(p==NULL){
        //ParamDec
        return f;
    }
    else{
        //ParamDec COMMA VarList
        if(f==NULL){
            f=VarList(p->brotherNode); 
        }
        else{
            f->next=VarList(p->brotherNode);
        }
    }
    return f;

}

FieldList ParamDec(struct typeNode *node){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    Type t=Specifier(p);
    if(t==NULL)return NULL;
    FieldList f=VarDec(p->brotherNode,t,2);
    return f;
}






Type StructSpecifier(struct typeNode* node){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    p=p->brotherNode;
    FieldList f=NULL;
    if(strcmp(p->type,"Tag")!=0){
        //STRUCT OptTag LC DefList RC
        char *str;
        FieldList stvar; 
        if(strcmp(p->type,"OptTag")!=0){
            str=NULL;
            f=DefList(p->brotherNode,1);
        }
        else{
            str=p->childNode->text;
            //insert struct into table
            stvar=(FieldList)malloc(sizeof(struct FieldList_));
            stvar->name=str;
            stvar->type=NULL;
            stvar->line=p->childNode->line;
            stvar->next=NULL;
            stvar->hashnext=NULL;
            if(insertVar(stvar)==1){
                printf("Error type 16 at Line %d:Duplicated name '%s'\n",stvar->line,stvar->name);
            }
            f=DefList(p->brotherNode->brotherNode,1);
        }
        //Figure out DefListf
        Type t=(Type)malloc(sizeof(struct Type_));
        t->kind=STRUCTURE;
        t->u.structure=f;
        //printfield(f);
        if(str!=NULL){
            stvar->type=t;
        }
        return t;
    }
    else {
        //STRUCT Tag
        char *str=p->childNode->text;           
        FieldList f=find_var(str);
        if(f==NULL){
        printf("Error Type 17 at Line %d:Undefined structure '%s'\n",p->childNode->line,str);
            return NULL;
        }
        else{
            return f->type;
         }
    } 
}

FieldList DefList(struct typeNode *node,int from){
    //DefList->NULL
    if(node==NULL)return NULL;
    struct typeNode* p=node->childNode;
    if(p==NULL)return NULL;
    //Def DefList
    FieldList f;
    f=Def(p,from);
    if(f==NULL){
        f=DefList(p->brotherNode,from);
    }
    else{
        FieldList ff=f;
        while(ff->next!=NULL)
        ff=ff->next;
        ff->next=DefList(p->brotherNode,from);
    }
    return f;
}

FieldList Def(struct typeNode *node,int from){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    Type t=Specifier(p);
    if(t==NULL)return NULL;
    //Specifier DecList SEMI
    FieldList f=DecList(p->brotherNode,t,from);
   // printfield(f);
    return  f;
}

FieldList DecList(struct typeNode *node,Type t,int from){
    if(node==NULL)return NULL;
    struct typeNode *p=node->childNode;
    FieldList f=Dec(p,t,from);
    if(p->brotherNode!=NULL){
        //Dec COMMA DecList
        if(f==NULL){
            f=DecList(p->brotherNode->brotherNode,t,from);
        }
        else{
        f->next=DecList(p->brotherNode->brotherNode,t,from);
    }
    }
    return f;
}

FieldList Dec(struct typeNode *node,Type t,int from){
    if(node==NULL)return NULL;
    struct typeNode* p=node->childNode;
    FieldList f=VarDec(p,t,from);
    if(p->brotherNode!=NULL){
    //VarDec ASSIGNOP EXP
        if(from==1){
            printf("Error type 18 at Line %d: struct cannot be initialized\n",p->line);
            return f;
        }
        else{
            p=p->brotherNode;
            Type tt=Exp(p->brotherNode);
            if(tt!=NULL&&t!=NULL&&isEqual(tt,t)==-1){
                printf("Error type 5 at Line %d:Type mismatched for assignment\n",p->line);
            }
        }
    }
    return f;
}




/****************for test*************/
void printstruct(Type t){
    printf("struct:{\n");
    printfield(t->u.structure);
    printf("}\n");
}


void printfield(FieldList f){
    if(f==NULL)return;
    if(f->type->kind==0){
       printf("name:%s\n",f->name);
       printf("Type:%d\n",f->type->kind);
       printf("line%d\n",f->line);
       printf("basic:%d\n",f->type->u.basic);
    }
    else
    if(f->type->kind==2){
        printstruct(f->type);
    }
    if(f->next!=NULL){
        printfield(f->next);
    }
    
}

void printtable(FieldList f[]){
    int i=0;
    for(;i<16384;i++){
    if(f[i]!=NULL){
        printfield(f[i]);
        printf("***************\n");
        while(f[i]->hashnext!=NULL){
            printf("new field*************\n");
            printfield(f[i]->hashnext);
            printf("************888888****\n");
        }
    }

}
}

void printtype(Type t){
        if(t->kind==BASIC&&t->u.basic==0)
        printf(" int ");
        else if(t->kind==BASIC&&t->u.basic==1)
        printf(" float ");
        else if(t->kind==ARRAY){
            printtype(t->u.array.elem);
            printf("[]");
        }
        else if(t->kind==STRUCTURE){
            printf("struct %s ",t->u.structure->name);
        }

}
void printParam(FieldList f){
    while(f!=NULL){
        Type t=f->type;
        printtype(t);
        f=f->next;
    }
}

void printArgs(struct typeNode *node){
        struct typeNode  *p=node->childNode;
        Type t=Exp(p);
        if(t==NULL)return;
        printtype(t);
        p=p->brotherNode;
        if(p==NULL)return;
        p=p->brotherNode;
        printArgs(p);
}







