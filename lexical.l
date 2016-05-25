%option yylineno
%{ 
    #include<stdlib.h>
    #include<stdio.h>
    #include<string.h>
	#include"syntax.tab.h"
    #include"typeNode.h"
    extern isError;
//	typedef enum{
//		false,true
//	}bool;
    int yycolumn=1;
 //	struct typeNode{
//		bool istoken;
//		int line;
  //      int firstcolumn,lastcolumn;
//		char type[16];
//		char text[32];
//		struct typeNode* childNode;
//		struct typeNode* brotherNode;
//		};

	void settypeNode(char *type,char *text);
%}
delim [ \t\r]
ws {delim}+
digit [0-9]
letter [A-Za-z_]
INT (0|([1-9]{digit}*))
FLOAT (({digit}*\.{digit}+)|({digit}+\.{digit}*))
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE (int)|(float)
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
ID {letter}({letter}|{digit})*
%%
\n         {yycolumn=1; }
{ws}       {/*do nothing*/}
{FLOAT}    {settypeNode("FLOAT",yytext);return FLOAT;}
{INT}      {settypeNode("INT",yytext);return INT;}
{STRUCT}   {settypeNode("STRUCT",yytext);return STRUCT;}
{RETURN}   {settypeNode("RETURN",yytext);return RETURN;}
{IF}       {settypeNode("IF",yytext);return IF;}
{ELSE}     {settypeNode("ELSE",yytext);return ELSE;}
{WHILE}    {settypeNode("WHILE",yytext);return WHILE;}
{TYPE}     {settypeNode("TYPE",yytext);return TYPE;}
{ID}       {settypeNode("ID",yytext);return ID;}
{SEMI}     {settypeNode("SEMI",yytext);return SEMI;}
{COMMA}    {settypeNode("COMMA",yytext);return COMMA;}
{ASSIGNOP} {settypeNode("ASSIGNOP",yytext);return ASSIGNOP;}
{RELOP}    {settypeNode("RELOP",yytext);return RELOP;}
{PLUS}     {settypeNode("ADD",yytext);return ADD;}
{MINUS}    {settypeNode("SUB",yytext);return SUB;}
{STAR}     {settypeNode("MUL",yytext);return MUL;}
{DIV}      {settypeNode("DIV",yytext);return DIV;}
{AND}      {settypeNode("AND",yytext);return AND;}
{OR}       {settypeNode("OR",yytext);return OR;}
{DOT}      {settypeNode("DOT",yytext);return DOT;}
{NOT}      {settypeNode("NOT",yytext);return NOT;}
{LP}       {settypeNode("LP",yytext);return LP;}
{RP}       {settypeNode("RP",yytext);return RP;}
{LB}       {settypeNode("LB",yytext);return LB;}
{RB}       {settypeNode("RB",yytext);return RB;}
{LC}       {settypeNode("LC",yytext);return LC;}
{RC}       {settypeNode("RC",yytext);return RC;}
"//"       {char c=input();
			while(c!='\n')c=input();}
"/*"       {char c=input();
            int flag=0;
			while(c!=EOF){
				if(c=='*'){
					char  d=input();
					if(d=='/'){
                        flag=1;
                        break;
                    }
					else c=d;
					}
				else c=input();
			}
            if((c==EOF)&&(flag==0)){
                isError=1;printf("Error type A at Line %d:comments met EOF\n",yylineno);
            }
           }
.       {isError=1; printf("Error type A at line %d: Mysterious character \'%s\'\n",yylineno,yytext);}
%%
void settypeNode(char *type,char *text){
	yylval.node=(struct typeNode*)malloc(sizeof(struct typeNode));
	strcpy(yylval.node->type,type);
	strcpy(yylval.node->text,text);
	yylval.node->istoken=true;
	yylval.node->line=yylineno;
    yylval.node->firstcolumn=yycolumn;
    yylval.node->lastcolumn=yycolumn+yyleng-1;
    yycolumn +=yyleng;
	yylval.node->childNode=NULL;
	yylval.node->brotherNode=NULL;
}
