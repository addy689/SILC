/*
	This file contains the Yacc file specification for implementing an expression tree for a calculator
*/

%{
	#include<stdio.h>
	#include<stdlib.h>
	#define ADD 1
	#define SUB 2
	#define MUL 3
	#define DIV 4
	
	struct Tnode {
			int val;
			char op;
			int flag;
			struct Tnode *l,*r;
	};

	void inorderTrav(struct Tnode *);
	void postorderTrav(struct Tnode *);
%}

%union {	struct Tnode *T;
			}
%start list
%token <T> DIGIT
%type <T> expr
%left <T> '+' '-'
%left <T> '*' '/'

%%
list	:	list expr '\n'
			{	printf("\nInOrder Traversal: ");
				inorderTrav($2);
				printf("\nPostOrder Traversal: ");
				postorderTrav($2);
				printf("\n");
				}
		|
		;

expr	:	expr '+' expr
			{	$$ = $2;
				$$->l = $1;
				$$->r = $3;
				}
		
		|	expr '-' expr
			{	$$ = $2;
				$$->l = $1;
				$$->r = $3;
				}
		
		|	expr '*' expr
			{	$$ = $2;
				$$->l = $1;
				$$->r = $3;
				}
		
		|	expr '/' expr
			{	$$ = $2;
				$$->l = $1;
				$$->r = $3;
				}
		
		|	'(' expr ')'
			{	$$ = $2;
				}
		
		|	DIGIT
			{	printf("Yo"); $$ = $1;
				}
		;

%%
#include "lex.yy.c"

int main()
{
	return yyparse();
}

void inorderTrav(struct Tnode *root)
{
	if(root!=NULL)
	{
		inorderTrav(root->l);
		
		if(root->flag!=0)
			printf("%c",root->op);
		else
			printf("%d",root->val);
		
		inorderTrav(root->r);
	}
}

void postorderTrav(struct Tnode *root)
{
	if(root!=NULL)
	{
		postorderTrav(root->l);
		postorderTrav(root->r);
		
		if(root->flag!=0)
			printf("%c",root->op);
		else
			printf("%d",root->val);
	}
}
