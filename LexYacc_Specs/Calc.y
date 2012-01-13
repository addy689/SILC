%{
	#include<stdio.h>
	#include<stdlib.h>
%}
%start list
%token DIGIT
%left '+' '-'
%left '*' '/'

%%
list	:	list expr '\n'
			{	printf("\nSum: %d\n", $2);
				}
		|
		;

expr	:	expr '+' expr 
			{	$$ = $1 + $3;
				printf("+");
				}
		
		|	expr '-' expr 
			{	$$ = $1 - $3;
				printf("-");
				}
		
		|	expr '*' expr 
			{	$$ = $1 * $3; 
				printf("*");
				}
		
		|	expr '/' expr 
			{	$$ = $1 / $3;
				printf("/");
				}
		
		|	'(' expr ')'
			{	$$ = $2;
				}
		
		|	'-' expr
			{	$$ = -1 * $2;
				}
		
		|	DIGIT
			{	$$ = $1;
				printf("%d",$$);
				}
		;

%%
#include "lex.yy.c"
int main()
{
	return yyparse();
}
