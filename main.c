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
		for(i=1;i<argc;i++){
			FILE* f=fopen(argv[i],"r");
			if(!f){ 
				perror(argv[i]);
				return 1;
			}
			yyrestart(f);
			yylineno=1;
			//yydebug=1;
			yyparse();
         //   fprintf(stderr,"%d",isError);
            if(isError==0){
               // printTree(root,0);

			    cc(root);
            }
			fclose(f);
		} 
	}
	else{
		printf("Stdin!");
		yyparse();

	}
	return 0;
}
