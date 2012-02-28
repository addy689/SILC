/*
	This file contains the Yacc file specification for constructing an expression tree for a SIL program
*/

%{
	#include "../LexYacc_Spec/SimpleCalc/CalcLibrary.h"
%}

%union {	struct Tnode *T;
			}

%start <T> prog
%token <T> READ WRITE DIGIT ID
%type <T> declbody declist decl identseq body stlist st expr
%right <T> '='
%left <T> '+' '-'
%left <T> '*' '/'

%%
prog		:	declbody '\n' body
				{	printf("\n--RUN--\n");
					ex($1);
					ex($3);
					}
			;

declbody	:	DECL '\n' stlist '\n' ENDDECL
				{	printf("\nPARSER: Found declbody : DECL stlist ENDDECL\n");
					$$ = $3;
					}
			;

body	:	BEGN '\n' stlist '\n' ED
			{	printf("\nPARSER: Found body : BEGN stlist END\n");
				$$ = $3;
				}
		;

stlist	:	stlist '\n' st
			{	printf("\nPARSER: Found stlist st\n");
				$$ = allocateNode(0,STLIST,"",0,$1,$3,NULL);
				$$->Ptr1 = $1;
				$$->Ptr2 = $3;
				}
		
		|	st
			{	printf("\nPARSER: Found st\n");
				$$ = $1;
				}
		
		;

st		:	ID '=' expr ';'
			{	printf("\nPARSER: Found ID = expr;\n");
				context_check($1);
				$$ = $2;
				$$->Ptr1 = $1;
				$$->Ptr2 = $3;
				insertSymTable($1->Gentry,$3->VALUE,ASSGN);
				}
		
		|	READ '(' ID ')' ';'
			{	printf("\nPARSER: Found READ (ID);\n");
				context_check($3);
				$$ = $1;
				$$->Ptr1 = $$->Ptr2 = $3;
				}
		
		|	WRITE '(' expr ')' ';'
			{	printf("\nPARSER: Found WRITE (ID);\n");
				$$ = $1;
				$$->Ptr1 = $$->Ptr2 = $3;
				}
		
		|	integer identseq ';'
			{	printf("\nPARSER: Found decl: integer identseq ;");
				//Mark types of all right children of $2 as integer
				marktype($2,INTGR);
				$$ = $2;
				}
		
		|	boolean identseq ';'
			{	printf("\nPARSER: Found decl: integer identseq ID ;");
				//Mark types of all right children of $2 as boolean
				marktype($2,BOOL);
				$$ = $2;
				}
		
		;

identseq	:	identseq ',' ID
				{	printf("\nPARSER: Found identseq: identseq ID");
					Ginstall($3->NAME,0,0,NULL);
					$$ = allocateNode(0,IDSEQ,"",0,$1,$3,NULL);
					}
			
			|	identseq ',' ID '[' DIGIT ']'
				{	printf("\nPARSER: Found identseq: identseq ID []");
					Ginstall($3->NAME,0,$5->VALUE,NULL);
					$$ = allocateNode(0,IDSEQ,"",0,$1,$3,NULL);
					}
			
			|	ID
				{	printf("\nPARSER: Found identseq: ID");
					Ginstall($1->NAME,0,0,NULL);
					$$ = allocateNode(0,IDSEQ,"",0,$1,$1,NULL);
					}
			
			|	ID '[' DIGIT ']'
				{	printf("\nPARSER: Found ID []");
					Ginstall($1->NAME,0,$3->VALUE,NULL);
					$$ = allocateNode(0,IDSEQ,"",0,$1,$1,NULL);
					}
			;

expr	:	expr '+' expr
			{	$$ = $2;
				$$->Ptr1 = $1;
				$$->Ptr2 = $3;
				}
		
		|	expr '-' expr
			{	$$ = $2;
				$$->Ptr1 = $1;
				$$->Ptr2 = $3;
				}
		
		|	expr '*' expr
			{	$$ = $2;
				$$->Ptr1 = $1;
				$$->Ptr2 = $3;
				}
		
		|	expr '/' expr
			{	$$ = $2;
				$$->Ptr1 = $1;
				$$->Ptr2 = $3;
				}
		
		|	'(' expr ')'
			{	$$ = $2;
				}
		
		|	DIGIT
			{	printf("\nPARSER: Found DIGIT\n");
				$$ = $1;
				}
		
		|	ID
			{	printf("\nPARSER: Found ID");
				context_check($1);
				$$ = $1;
				}
		
		|	ID '[' expr ']'
			{
				printf("\nPARSER: Found ID []");
				context_check($1);
				$$ = $1;
				$$->Ptr1 = $$->Ptr2 = $3;
				}
		
		;

%%
#include "lex.yy.c"

int main(int argc,char *argv[])
{
	yyin = fopen(argv[1],"Ptr2");
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
	
	switch(root->NODETYPE)
	{
		case STLIST:	ex(root->Ptr1);
						ex(root->Ptr2);
						break;
		
		case RD:		insertSymTable((root->Ptr1)->binding,0,RD);
						return 0;
		
		case WRT:		printf("%d\n",ex(root->Ptr1));
						return 0;
		
		case ASSGN:		return sym[(root->Ptr1)->binding] = ex(root->Ptr2);
		
		case ADD:		return ex(root->Ptr1) + ex (root->Ptr2);
		
		case SUB:		return ex(root->Ptr1) - ex (root->Ptr2);
		
		case MUL:		return ex(root->Ptr1) * ex (root->Ptr2);
		
		case DIV:		return ex(root->Ptr1) / ex (root->Ptr2);
		
		case ID:		return sym[root->binding];
		
		case NUM:		return root->val;
		
		default:		printf("How did flag get this value!");
	}
}
