/*
	This file contains the Yacc specification for obtaining POSTFIX notation of an expression (without computing the expression tree).
*/

%{
	#include<stdio.h>
	#include<string.h>
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
			{	postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"+");
				strcpy($$,postfix);
				}
		
		|	expr '-' expr 
			{	postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"-");
				strcpy($$,postfix);
				}
		
		|	expr '*' expr 
			{	postfix[0] = '\0';
				strcat(postfix,$1);
				strcat(postfix,$3);
				strcat(postfix,"*");
				strcpy($$,postfix);
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
{ctr=0;
	return yyparse();
}
