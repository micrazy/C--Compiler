/*************************************************************************
	> File Name: cc.h
	> Author: Micrazy
	> Mail: micrazy@live.com 
	> Created Time: Tue 19 Apr 2016 03:16:10 PM CST
 ************************************************************************/
#include"typeNode.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Functype_* Functype;

struct Type_{
    enum{BASIC,ARRAY,STRUCTURE}kind;
    union{
        int basic;//basic
        struct {Type elem;int size;}array;
        FieldList structure;
    }u;
};
struct FieldList_{
    char * name;
    Type type;
    int line;
    FieldList next;//next*
    FieldList hashnext; //hashtable pointer
};

struct Functype_{//record Func
    char * name;
    int line;
    Type returnType;//type 0f return 
    FieldList param;//params
    Functype hashnext;//hashtable pointer
};


unsigned int hash_pjw(char *name);
void cc(struct typeNode *root);
int insertVar(FieldList f);
int insertFun(Functype f);
int insertParam(FieldList f);
FieldList find_var(char *name);
void Program(struct typeNode *root);
void ExtDefList(struct typeNode *node);
void ExtDef(struct typeNode *node);
Type Specifier(struct typeNode *node);
Type StructSpecifier(struct typeNode* node);
void ExtDecList(struct typeNode *node,Type t);
FieldList VarDec(struct typeNode *node,Type t,int from);
Functype FunDec(struct typeNode *node,Type t);
void CompSt(struct typeNode *node,Type f);
FieldList VarList(struct typeNode *node);
FieldList ParamDec(struct typeNode *node);
FieldList DefList(struct typeNode *node,int from);
FieldList Def(struct typeNode *node,int from);
FieldList DecList(struct typeNode *node,Type t,int from);
FieldList Dec(struct typeNode* node,Type t,int from);
void StmtList(struct typeNode *node,Type t);
void Stmt(struct typeNode *node,Type t);
Type Exp(struct typeNode *node);
int Args(struct typeNode *node,FieldList f);
int isEqual(Type t1,Type t2);
int equalStruct(FieldList s1,FieldList s2);


void printfield(FieldList f);
void printtable(FieldList f[]);
void printNode(struct typeNode *node);
void printParam(FieldList f);
void printArgs(struct typeNode *node);
