/*
	This file contains the Yacc specification for obtaining POSTFIX notation of an expression
*/

%{
	#include<stdio.h>
	#include<string.h>
	#include<stdlib.h>
	char postfix[300];
	int ctr;
%}
%union { char value[300]; }
%start list
%token <value> DIGIT
%type <value> expr
%left '+' '-'
%left '*' '/'

%%
list	:	list expr '\n'
			{	printf("\nPostfix: %s\n", $2);
				}
		|
		;

expr	:	expr '+' expr
			{	printf("\nPARSER: Found E+E");
				printf("\n$$ = %s",$$);
				printf("\n$1 = %s",$1);
				printf("\n$3 = %s",$3);
				
				postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"+");
				strcpy($$,postfix);
				printf("\n$$ = %s",$$);
				}
		
		|	expr '-' expr 
			{	printf("\nPARSER: Found E-E");
				printf("\n$$ = %s",$$);
				printf("\n$1 = %s",$1);
				printf("\n$3 = %s",$3);
				
				postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"-");
				strcpy($$,postfix);
				printf("\n$$ = %s",$$);
				}
		
		|	expr '*' expr 
			{	printf("\nPARSER: Found E*E");
				printf("\n$$ = %s",$$);
				printf("\n$1 = %s",$1);
				printf("\n$3 = %s",$3);
				
				postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"*");
				strcpy($$,postfix);
				printf("\n$$ = %s",$$);
				}
		
		|	expr '/' expr 
			{	postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"/");
				strcpy($$,postfix);
				}
		
		|	'(' expr ')'
			{	strcpy($$,$2);
				}
		

		|	DIGIT
			{	strcpy($$,$1);
				}
		;

%%
#include "lex.yy.c"
int main()
{
	ctr=0;
	return yyparse();
}
