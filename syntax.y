%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
    #include"lex.yy.c"
	#include<stdarg.h>
    #define YYERROR_VERBOSE 
    #define YYDEBUG 1
	struct typeNode *root;
    int isError=0;
    char errormsg[20];
	struct typeNode *addToTree(char *type,int num,...);
	void printTree(struct typeNode *p,int depth);
%}
%union {struct typeNode *node;}
/*declared tokens*/
%token <node> INT
%token <node> FLOAT 
%token <node>TYPE SEMI COMMA ID  
%right <node>ASSIGNOP
%left <node>OR
%left <node>AND
%left <node>RELOP
%left <node>ADD SUB
%left <node>MUL DIV
%right <node>NOT
%left <node>LP RP LB RB LC RC DOT
%nonassoc <node>LOWER_THAN_ELSE
%nonassoc <node>STRUCT RETURN IF ELSE WHILE
/*declared non-terminals*/
%type <node>Program ExtDefList ExtDef ExtDecList
%type <node>Specifier StructSpecifier OptTag Tag
%type <node>VarDec FunDec VarList ParamDec
%type <node>CompSt StmtList Stmt
%type <node>DefList Def DecList Dec
%type <node>Exp Args
%% 
Program : ExtDefList                                   {$$=addToTree("Program",1,$1);root=$$;}
  ;                                                 
ExtDefList : ExtDef ExtDefList                         {$$=addToTree("ExtDefList",2,$1,$2);}
  | /*null*/                                           {$$=NULL;}
  ;
ExtDef : Specifier ExtDecList SEMI                     {$$=addToTree("ExtDef",3,$1,$2,$3);}
  | Specifier SEMI                                     {$$=addToTree("ExtDef",2,$1,$2);}
  | error     SEMI                                     {isError=1;}
  | Specifier error                                    {isError=1;}
  | Specifier error SEMI                               {isError=1;}
  | Specifier ExtDecList error                         {isError=1;}
  | Specifier FunDec CompSt                            {$$=addToTree("ExtDef",3,$1,$2,$3);}
  | Specifier FunDec error SEMI                              {isError=1;}
  ;
ExtDecList : VarDec                                    {$$=addToTree("ExtDecList",1,$1);}
  | VarDec COMMA ExtDecList                            {$$=addToTree("ExtDecList",3,$1,$2,$3);}
  ;
Specifier : TYPE                                       {$$=addToTree("Specifier",1,$1);}
  | StructSpecifier                                    {$$=addToTree("Specifier",1,$1);}
  ;
StructSpecifier :STRUCT OptTag LC DefList RC           {$$=addToTree("StructSpecifier",5,$1,$2,$3,$4,$5);}
  | STRUCT Tag                                         {$$=addToTree("StructSpecifier",2,$1,$2);}
  | STRUCT error RC                                    {isError=1;}
  ;
OptTag : ID                                            {$$=addToTree("OptTag",1,$1);}
  | /*null*/                                           {$$=NULL;}
  ;
Tag : ID                                               {$$=addToTree("Tag",1,$1);}
  ;
VarDec : ID                                            {$$=addToTree("VarDec",1,$1);}
  | VarDec LB INT RB                                   {$$=addToTree("VarDec",4,$1,$2,$3,$4);}              
  | error RB                                           {isError=1;}
  ;
FunDec : ID LP VarList RP                              {$$=addToTree("FunDec",4,$1,$2,$3,$4);}
  | ID LP RP                                           {$$=addToTree("FunDec",3,$1,$2,$3);}
  | ID LP error RP                                     {isError=1;}
  ;
VarList : ParamDec COMMA VarList                       {$$=addToTree("VarList",3,$1,$2,$3);}
  | ParamDec                                           {$$=addToTree("VarList",1,$1);}
  ;
ParamDec : Specifier VarDec                            {$$=addToTree("ParamDec",2,$1,$2);}
  ;
CompSt : LC DefList StmtList RC                        {$$=addToTree("CompSt",4,$1,$2,$3,$4);}
  | error RC                                           {isError=1;}
  ;
StmtList : Stmt StmtList                               {$$=addToTree("StmtList",2,$1,$2);}
  | /*null*/                                           {$$=NULL;}
  ;
Stmt : Exp SEMI                                        {$$=addToTree("Stmt",2,$1,$2);}
  | CompSt                                             {$$=addToTree("Stmt",1,$1);}
  | RETURN Exp SEMI                                    {$$=addToTree("Stmt",3,$1,$2,$3);}
  | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE            {$$=addToTree("Stmt",5,$1,$2,$3,$4,$5);}
  | IF LP Exp RP Stmt ELSE Stmt                        {$$=addToTree("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
  | WHILE LP Exp RP Stmt                               {$$=addToTree("Stmt",5,$1,$2,$3,$4,$5);}
  | error SEMI                                         {isError=1;}
  | Exp error                                          {isError=1;}
  ;
DefList : Def DefList                                  {$$=addToTree("DefList",2,$1,$2);}
  | /*null*/                                           {$$=NULL;}   
  ;
Def : Specifier DecList SEMI                           {$$=addToTree("Def",3,$1,$2,$3);}
  | Specifier DecList error                            {isError=1;}
  ;
DecList : Dec                                          {$$=addToTree("DecList",1,$1);}
  | Dec COMMA DecList                                  {$$=addToTree("DecList",3,$1,$2,$3);}
  ;
Dec : VarDec                                           {$$=addToTree("Dec",1,$1);}
  | VarDec ASSIGNOP Exp                                {$$=addToTree("Dec",3,$1,$2,$3);}
  ;
Exp : Exp ASSIGNOP Exp                                 {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp AND Exp                                        {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp OR Exp                                         {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp RELOP Exp                                      {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp ADD Exp                                        {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp SUB Exp                                        {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp MUL Exp                                        {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp DIV Exp                                        {$$=addToTree("Exp",3,$1,$2,$3);}
  | LP Exp RP                                          {$$=addToTree("Exp",3,$1,$2,$3);}
  | SUB Exp                                            {$$=addToTree("Exp",2,$1,$2);}
  | NOT Exp                                            {$$=addToTree("Exp",2,$1,$2);}
  | ID LP Args RP                                      {$$=addToTree("Exp",4,$1,$2,$3,$4);}
  | ID LP RP                                           {$$=addToTree("Exp",3,$1,$2,$3);}
  | Exp LB Exp RB                                      {$$=addToTree("Exp",4,$1,$2,$3,$4);}
  | Exp DOT ID                                         {$$=addToTree("Exp",3,$1,$2,$3);}
  | ID                                                 {$$=addToTree("Exp",1,$1);}
  | INT                                                {$$=addToTree("Exp",1,$1);}
  | FLOAT                                              {$$=addToTree("Exp",1,$1);}
  | error RP                                           {isError=1;}
  | Exp LB error RB                                    {isError=1;}
  | ADD error                                       {isError=1;} 
  ;
Args : Exp COMMA Args                                  {$$=addToTree("Args",3,$1,$2,$3);} 
  | Exp                                                {$$=addToTree("Args",1,$1);}
  ;
%%
yyerror(char *msg){
	fprintf(stderr,"Error type B at %d.%d-%d: %s\n",yylineno,yylval.node->firstcolumn,yylval.node->lastcolumn,msg);
}
struct typeNode *addToTree(char *p,int num,...){
	struct typeNode *cur=(struct typeNode*)malloc(sizeof(struct typeNode));
	struct typeNode *tmp=(struct typeNode*)malloc(sizeof(struct typeNode));
	cur->istoken=false;
	
	va_list nodeList;
	va_start(nodeList,num);
	tmp=va_arg(nodeList,struct typeNode*);
    if(tmp!=NULL){
	cur->line=tmp->line;
	strcpy(cur->type,p);
	cur->childNode=tmp;
    }
    else return tmp;
	int i=1;
	for(;i<num;i++){
		struct typeNode* tmp1=va_arg(nodeList,struct typeNode*);
		if(tmp1!=NULL){
			tmp->brotherNode=tmp1;
			tmp=tmp1;
		}	
	}
	tmp->brotherNode=NULL;

	va_end(nodeList);
	return cur;
}
void printTree(struct typeNode *p,int depth){
	if(p==NULL)return ;
	int i;
	for(i=0;i<depth;i++)
		printf("  ");
	if(p->istoken==false){
		printf("%s (%d)\n",p->type,p->line);
		printTree(p->childNode,depth+1);
	}
	else{
		if(strcmp(p->type,"INT")==0){
			printf("%s: %d\n",p->type,atoi(p->text));
		}
		else if(strcmp(p->type,"FLOAT")==0)
				printf("%s: %f\n",p->type,atof(p->text));
		else if((strcmp(p->type,"TYPE")==0)||(strcmp(p->type,"ID")==0))
				printf("%s: %s\n",p->type,p->text);
		else printf("%s\n",p->type);
	}
	printTree(p->brotherNode,depth);
			
}

