#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"cc.h"
 void dd(struct typeNode*  node);
 void tProgram(struct typeNode* root);
 void tExtDefList(struct typeNode* node);
 void tExtDef(struct typeNode *node);
 void tExtDecList(struct typeNode *node);
 void tVarDec(struct typeNode *node,int from);
 void tCompSt(struct typeNode *node);
 void tVarList(struct typeNode *node);
 void tParamDec(struct typeNode *node);
 void tDefList(struct typeNode *node,int from);
 void tDef(struct typeNode *node,int from);
 void tDecList(struct typeNode *node,int from);
 void tDec(struct typeNode* node,int from);
 void tStmtList(struct typeNode *node);
 void tStmt(struct typeNode *node);
 void tExp(struct typeNode *node,Operand o);
 void tExp_Cond();
 void tArgs();

