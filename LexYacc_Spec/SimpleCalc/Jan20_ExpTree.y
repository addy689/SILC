/*
	This file contains the Yacc file specification for constructing an expression tree for a SIL program
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

%token <T> DIGIT VAR READ
//begin end
%type <T> expr st stlist
%right <T> '='
%left <T> '+' '-'
%left <T> '*' '/'

%%
//prog	:	begin stlist end
//			{	printf("\nInOrder Traversal: ");
//				inorderTrav($2);
//				printf("\n");
//				}
//		;

stlist	:	stlist st
			{	printf("\nPARSER: Found stlist st\n");
				$$ = allocateNode(0,STLIST,'\0',-1);
				$$->l = $1;
				$$->r = $2;
				}
		
		|	st
			{	printf("\nPARSER: Found st\n");
				$$ = $1;
				}
		
		;

st		:	VAR '=' expr ';' '\n'
			{	//store expr in symbol table array
				printf("\nPARSER: Found VAR = expr;\n");
				$$ = $2;
				$$->l = $1;
				$$->r = $3;
				}
		
		|	READ '(' VAR ')' ';' '\n'
			{	printf("\nPARSER: Found READ (VAR);\n");
				$$ = $1;
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
			{	printf("\nPARSER: Found DIGIT\n");
				$$ = $1;
				}
		
		|	VAR
			{	printf("\nPARSER: Found VAR\n");
				$$ = $1;
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
