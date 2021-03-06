#include<stdio.h>
#include"cc.h"
extern FILE *yyin;
extern int yylineno;
extern int yydebug;
extern struct typeNode* root;
extern int isError;
extern void printTree();
extern void yyparse();
extern void yyrestart();
int main(int argc,char** argv){
	int i=0;
	if(argc>1){
			FILE* f=fopen(argv[1],"r");
			if(!f){ 
				perror(argv[1]);
				return 1;
			}
			yyrestart(f);
			yylineno=1;
			//yydebug=1;
			yyparse();
         //   fprintf(stderr,"%d",isError);
            if(isError==0){
            //    printTree(root,0);

			    cc(root,argv[2],argv[3]);
            }
			fclose(f);
	}
	else{
		printf("Stdin!");
		yyparse();

	}
	return 0;
}
