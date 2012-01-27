/*
	This file contains the Yacc file specification for implementing a simple calculator and an expression tree
*/

%{
	#include<stdio.h>
	#include<stdlib.h>
	#define NUM 0
	#define ADD 1
	#define SUB 2
	#define MUL 3
	#define DIV 4
	#define ASSGN 5
	#define VRBL 6
	#define RD 7
	#define STLIST 8
	
	struct Tnode {
			int val;
			char op;
			int flag;
			int binding;
			struct Tnode *l,*r;
	};

	struct Tnode* allocateNode(int,int,char,int);
	void inorderTrav(struct Tnode *);
	void postorderTrav(struct Tnode *);
%}

%union {	struct Tnode *T;
			}
%start prog
%token <T> DIGIT VAR begin end READ
%type <T> expr stlist st
%right <T> '='
%left <T> '+' '-'
%left <T> '*' '/'

%%
prog	:	begin stlist end
			{	printf("\nInOrder Traversal: ");
				inorderTrav($2);
				printf("\n");
				}
		;

stlist	:	stlist st
			{	printf("Hangover");
				$$ = allocateNode(0,STLIST,'\0',-1);
				$$->l = $1;
				$$->r = $2;
				}
		
		|	st
			{	$$ = $1;printf("Hangover");
				}
		
		;

st		:	VAR '=' expr ';'
			{	//store expr in symbol table array
				$$ = $2;printf("Hangover");
				$$->l = $1;
				$$->r = $3;
				}
		
		|	READ '(' VAR ')' ';'
			{	$$ = $1;printf("Hangover");
				$$->l = $3;
				}
		
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
			{	$$ = $1;
				}
		
		|	VAR
			{	printf("\nYo\n");
				$$ = $1;
				}
		
		;

%%
#include "lex.yy.c"

int main(int argc,char *argv[])
{
//	yyin = fopen(argv[1],"r");
	
	yyparse();
	
//	fclose(yyin);
	return 0;
}

void inorderTrav(struct Tnode *root)
{
	printf("\n");
	if(root!=NULL)
	{
		inorderTrav(root->l);
		
		printf("%d",root->flag);
		
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
