/*
	This file contains the Yacc file specification for implementing a simple calculator and an expression tree
*/

%{
	#include<stdio.h>
	#include<stdlib.h>
	#define ADD 1
	#define SUB 2
	#define MUL 3
	#define DIV 4
	
	typedef struct Tnode {
			int val;
			char op;
			int flag;
			struct Tnode *l,*r;
	}NODE;

%}
%union {	NODE *T;
			}
%start list
%token <T> DIGIT
%type <T> expr
%left '+' '-'
%left '*' '/'

%%
list	:	list expr '\n'
			{	printf("\nResult = %d\n", $2);
				}
		|
		;

expr	:	expr '+' expr
			{	$$->l = $1;
				$$->r = $3;
				}
		
		|	expr '-' expr
			{	$$->l = $1;
				$$->r = $3;
				}
		
		|	expr '*' expr
			{	$$->l = $1;
				$$->r = $3;; 
				}
		
		|	expr '/' expr
			{	$$->l = $1;
				$$->r = $3;;
				}
		
		|	'(' expr ')'
			{	$$ = $2;
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
