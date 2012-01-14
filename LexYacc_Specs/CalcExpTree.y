/*
	This file contains the Yacc file specification for implementing a simple calculator and an expression tree
*/

%{
	#include<stdio.h>
		
	struct Tnode {
			int val;
			char op;
			int flag;
			struct Tnode *l,*r;
	}

%}
%union {	struct Tnode *T;
			}
%start list
%token <Tnode *T> DIGIT
%type <Tnode *T> expr
%left '+' '-'
%left '*' '/'

%%
list	:	list expr '\n'
			{	printf("\nResult = %d\n", $2);
				}
		|
		;

expr	:	expr '+' expr 
			{	$$ = $1 + $3;
				}
		
		|	expr '-' expr 
			{	$$ = $1 - $3;
				}
		
		|	expr '*' expr 
			{	$$ = $1 * $3; 
				}
		
		|	expr '/' expr 
			{	$$ = $1 / $3;
				}
		
		|	'(' expr ')'
			{	$$ = $2;
				}
		
		|	'-' expr
			{	$$ = -1 * $2;
				}
		
		|	DIGIT
			{	$$ = $1;
				}
		;

%%
#include "lex.yy.c"
int main()
{
	return yyparse();
}
