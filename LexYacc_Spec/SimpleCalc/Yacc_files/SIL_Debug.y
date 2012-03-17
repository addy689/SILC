/*
	This file contains the Yacc file specification for constructing an expression tree for a SIL program
*/

%{
	#include "../LexYacc_Spec/SimpleCalc/CalcLibrary.h"
	char tmp[300]="";
%}

%union {	char c[300];
			}

%start prog
%token <c> integer boolean DECL ENDDECL DIGIT ID READ WRITE BEGN ED IF THEN ENDIF ELSE
%type <c> declbody identseq expr st stlist body

%left '>' '<'
%right '='
%left '+' '-'
%left '*' '/'
%%

prog		:	declbody '\n' body
				{	printf("\n\n--START--\n\n%s\n\n%s\n\n",$1,$3);
					}
			;

declbody	:	DECL '\n' stlist '\n' ENDDECL
				{	printf("\nPARSER: Found declbody: DECL declist ENDDECL");
					tmp[0]='\0';
					strcat(tmp,"DECL");
					strcat(tmp,"\n");
					strcat(tmp,$3);
					strcat(tmp,"\n");
					strcat(tmp,"ENDDECL");
					strcpy($$,tmp);
					}
			;

body	:	BEGN '\n' stlist '\n' ED
			{	printf("\nPARSER: Found BEGN stlist ED");
				tmp[0]='\0';
				strcat(tmp,"BEGIN");
				strcat(tmp,"\n");
				strcat(tmp,$3);
				strcat(tmp,"\nEND");
				strcpy($$,tmp);
				}
		;

stlist	:	stlist '\n' st
			{	printf("\nPARSER: Found stlist st\n");
				tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"\n");
				strcat(tmp,$3);
				strcpy($$,tmp);
				}
		
		|	st
			{	printf("\nPARSER: Found st\n");
				strcpy($$,$1);
				}
		
		;

st		:	IF '(' expr ')' THEN '\n' stlist '\n' ELSE '\n' stlist '\n' ENDIF ';'
			{	printf("\nPARSER: Found IF ELSE ENDIF\n");
				tmp[0]='\0';
				strcat(tmp,"IF");
				strcat(tmp,"(");
				strcat(tmp,$3);
				strcat(tmp,")");
				strcat(tmp,"THEN\n");
				strcat(tmp,$7);
				strcat(tmp,"\nELSE\n");
				strcat(tmp,$11);
				strcat(tmp,"\nENDIF;");
				strcpy($$,tmp);
				}
		
		|	IF '(' expr ')' THEN '\n' stlist '\n' ENDIF ';'
			{	printf("\nPARSER: Found IF ENDIF\n");
				tmp[0]='\0';
				strcat(tmp,"IF");
				strcat(tmp,"(");
				strcat(tmp,$3);
				strcat(tmp,")");
				strcat(tmp,"THEN\n");
				strcat(tmp,$7);
				strcat(tmp,"\nENDIF;");
				strcpy($$,tmp);
				}
		
		|	ID '=' expr ';'
			{	printf("\nPARSER: Found ID = expr;\n");
				tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"=");
				strcat(tmp,$3);
				strcat(tmp,";");
				strcpy($$,tmp);
				}
		
		|	READ '(' ID ')' ';'
			{	printf("\nPARSER: Found READ (ID);\n");
				tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"(");
				strcat(tmp,$3);
				strcat(tmp,");");
				strcpy($$,tmp);
				}
		
		|	WRITE '(' expr ')' ';'
			{	printf("\nPARSER: Found WRITE (ID);\n");
				tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"(");
				strcat(tmp,$3);
				strcat(tmp,");");
				strcpy($$,tmp);
				}
				
		|	integer identseq ';'
			{		printf("\nPARSER: Found decl: integer identseq ID ;");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,$2);
					strcat(tmp,";");
					strcpy($$,tmp);
					printf("\n%s",$$);
					}
		
		|	boolean identseq ';'
			{		printf("\nPARSER: Found decl: integer identseq ID ;");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,$2);
					strcat(tmp,";");
					strcpy($$,tmp);
					printf("\n%s",$$);
					}
		
		;

identseq	:	identseq ',' ID
				{	
					printf("\nPARSER: Found identseq: identseq ID ,");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,",");
					strcat(tmp,$3);
					strcpy($$,tmp);
					printf("\n%s",$$);
					}
			
			|	identseq ',' ID '[' DIGIT ']'
				{	printf("\nPARSER: Found identseq: identseq ID [] ,");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,",");
					strcat(tmp,$3);
					strcat(tmp,"[");
					strcat(tmp,$5);
					strcat(tmp,"]");
					strcpy($$,tmp);
					printf("\n%s",$$);
					}
			
			|	ID
				{	printf("\nPARSER: Found identseq: ID");
					strcpy($$,$1);
					}
			
			|	ID '[' DIGIT ']'
				{	printf("\nPARSER: Found ID []");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,"[");
					strcat(tmp,$3);
					strcat(tmp,"]");
					strcpy($$,tmp);
					}
			;

expr	:	expr '>' expr
			{	tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,">");
				strcat(tmp,$3);
				strcpy($$,tmp);
				}
		|
			expr '<' expr
			{	tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"<");
				strcat(tmp,$3);
				strcpy($$,tmp);
				}
		
		|	expr '-' expr
			{	tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"-");
				strcat(tmp,$3);
				strcpy($$,tmp);
				}
		
		|	expr '*' expr
			{	tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"*");
				strcat(tmp,$3);
				strcpy($$,tmp);
				}
		
		|	expr '/' expr
			{	tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"/");
				strcat(tmp,$3);
				strcpy($$,tmp);
				}
		
		|	'(' expr ')'
			{	tmp[0]='\0';
				strcat(tmp,"(");
				strcat(tmp,$2);
				strcat(tmp,")");
				strcpy($$,tmp);
				}
		
		|	DIGIT
			{	printf("\nPARSER: Found DIGIT\n");
				strcpy($$,$1);
				}
		
		|	ID
			{	printf("\nPARSER: Found ID\n");
				strcpy($$,$1);
				}
		
		|	ID '[' expr ']'
			{	printf("\nPARSER: Found ID\n");
				tmp[0]='\0';
				strcat(tmp,$1);
				strcat(tmp,"[");
				strcat(tmp,$3);
				strcat(tmp,"]");
				strcpy($$,tmp);
				}
		;

%%
#include "lex.yy.c"

int main(int argc,char *argv[])
{
	yyin = fopen(argv[1],"r");
	yyparse();
	fclose(yyin);
	return 0;
}
