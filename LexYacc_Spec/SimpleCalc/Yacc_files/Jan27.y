/*
	This file contains the Yacc file specification for constructing an expression tree for a SIL program
*/

%{
	#include "../LexYacc_Spec/SimpleCalc/CalcLibrary.h"
%}

%union {	struct Tnode *T;
			}

%start prog
%token <T> DIGIT VAR READ WRITE BEGN ED
%type <T> expr st stlist
%right <T> '='
%left <T> '+' '-'
%left <T> '*' '/'

%%
prog	:	BEGN '\n' stlist ED
			{	printf("\n--RUN--\n");
				ex($3);
				}
		;

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
			{	printf("\nPARSER: Found VAR = expr;\n");
				$$ = $2;
				$$->l = $1;
				$$->r = $3;
				insertSymTable($1->binding,$3->val,0);
				}
		
		|	READ '(' VAR ')' ';' '\n'
			{	printf("\nPARSER: Found READ (VAR);\n");
				$$ = $1;
				$$->l = $$->r = $3;
				}
		
		|	WRITE '(' expr ')' ';' '\n'
			{	printf("\nPARSER: Found WRITE (VAR);\n");
				$$ = $1;
				$$->l = $$->r = $3;
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

void insertSymTable(int x,int val,int fl)
{
	if(fl == RD)
		scanf("%d",&val);
		
	sym[x] = val;
}

int ex(struct Tnode *root)
{
	if(!root)
		return 0;
	
	switch(root->flag)
	{
		case STLIST:	ex(root->l);
						ex(root->r);
						break;
		
		case RD:		insertSymTable((root->l)->binding,0,RD);
						return 0;
		
		case WRT:		printf("%d\n",ex(root->l));
						return 0;
		
		case ASSGN:		return sym[(root->l)->binding] = ex(root->r);
		
		case ADD:		return ex(root->l) + ex (root->r);
		
		case SUB:		return ex(root->l) - ex (root->r);
		
		case MUL:		return ex(root->l) * ex (root->r);
		
		case DIV:		return ex(root->l) / ex (root->r);
		
		case VRBL:		return sym[root->binding];
		
		case NUM:		return root->val;
		
		default:		printf("How did flag get this value!");
	}
}
