/*
	This file contains the Yacc specification for obtaining PREFIX notation of an expression.(without computing the expression tree).
*/

%{
	#include<stdio.h>
	#include<string.h>
	char prefix[300];
%}
%union {	char value[300];
			}
%start list
%token <value> DIGIT
%type <value> expr
%left '+' '-'
%left '*' '/'

%%
list	:	list expr '\n'
			{	printf("\nPrefix: %s\n", $2);
				}
		|
		;

expr	:	expr '+' expr 
			{	prefix[0] = '\0';
				strcat(prefix,"+");
				strcat(prefix,$1);
				strcat(prefix,$3);
				strcpy($$,prefix);
				}
		
		|	expr '-' expr 
			{	prefix[0] = '\0';
				strcat(prefix,"-");
				strcat(prefix,$1);
				strcat(prefix,$3);
				strcpy($$,prefix);
				}
		
		|	expr '*' expr 
			{	prefix[0] = '\0';
				strcat(prefix,"*");
				strcat(prefix,$1);
				strcat(prefix,$3);
				strcpy($$,prefix);
				}
		
		|	expr '/' expr 
			{	prefix[0] = '\0';
				strcat(prefix,"/");
				strcat(prefix,$1);
				strcat(prefix,$3);
				strcpy($$,prefix);
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
	return yyparse();
}
