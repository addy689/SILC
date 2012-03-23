/*
	This file contains the Yacc file specification for constructing an expression tree for a SIL program
*/

%{
	#include "../SILCompiler/compilerLib.h"
%}

%union {	struct tnode *T;
			int intval;
			char *id;
			}

%start prog
%token <intval> DIGIT
%token <id> ID

%token READ WRITE BEGN END DECL ENDDECL INTEGER BOOLEAN
%type <T> declstmnt declist type identlist identname stlist statement expr
%right '='
%left '+' '-'
%left '*' '/'

%%
prog	:	DECL declist ENDDECL BEGN stlist END
			{	printf("\n--RUN--\n");
				compile($2,$5);
				}
		;

declist	:	declist declstmnt
			{	printf("\nPARSER: Found declist : declist declstmnt\n");
				$$ = TreeCreate(0,CONTINUE,"",0,$1,$2,NULL);
				}
		
		|	declstmnt
			{	printf("\nPARSER: Found declist : declstmnt\n");
				$$ = TreeCreate(0,CONTINUE,"",0,$1,NULL,NULL);
				}
		
		|	{	$$ = NULL;
				}
		
		;

declstmnt	:	type identlist ';'
				{	printf("\nPARSER: Found declstmnt : type identlist ;\n");
					$$ = TreeCreate(0,DECLSTATEMENT,"",0,$1,$2,NULL);
					}
			
			;

type	:	INTEGER
			{	printf("\nPARSER: Found type: INTEGER");
				$$ = TreeCreate(INTGR,DATATYPE,"",0,NULL,NULL,NULL);
				}
		
		|	BOOLEAN
			{	printf("\nPARSER: Found type: BOOLEAN");
				$$ = TreeCreate(BOOL,DATATYPE,"",0,NULL,NULL,NULL);
				}
		
		;

stlist	:	stlist statement
			{	printf("\nPARSER: Found stlist statement\n");
				$$ = TreeCreate(0,CONTINUE,"",0,$1,$2,NULL);
				}
		
		|	statement
			{	printf("\nPARSER: Found statement\n");
				$$ = TreeCreate(0,CONTINUE,"",0,$1,NULL,NULL);
				}
		
		|	{	$$ = NULL;
				}
		
		;

statement	:	ID '=' expr ';'
				{	printf("\nPARSER: Found ID = expr;\n");
					$$ = TreeCreate(0,ASSIGN,$1,0,$3,NULL,NULL);
					}
		
			|	ID '[' expr ']' '=' expr ';'
				{	printf("\nPARSER: Found ID[expr] = expr;\n");
					$$ = TreeCreate(0,ARRAYASSIGN,$1,0,$3,$6,NULL);
					}
		
			|	READ '(' ID ')' ';'
				{	printf("\nPARSER: Found statement: READ (ID);\n");
					$$ = TreeCreate(0,RD,$3,0,NULL,NULL,NULL);
					}
		
			|	READ '(' ID '[' expr ']' ')' ';'
				{	printf("\nPARSER: Found statement: READ (ID[expr]);\n");
					$$ = TreeCreate(0,ARRAYRD,$3,0,$5,NULL,NULL);
					}
		
			|	WRITE '(' expr ')' ';'
				{	printf("\nPARSER: Found statement: WRITE (expr);\n");
					$$ = TreeCreate(0,WRIT,"",0,$3,NULL,NULL);
					}
		
			;

identlist	:	identlist ',' identname
				{	printf("\nPARSER: Found identseq: identseq identname");
					$$ = TreeCreate(0,CONTINUE,"",0,$1,$3,NULL);
					}
			
			|	identname
				{	printf("\nPARSER: Found identseq: identname");
					$$ = TreeCreate(0,CONTINUE,"",0,$1,NULL,NULL);
					}
			
			;

identname	:	ID
				{	printf("\nPARSER: Found identname: ID");
					$$ = TreeCreate(0,IDFRDECL,$1,0,NULL,NULL,NULL);
					}
			
			|	ID '[' DIGIT ']'
				{	printf("\nPARSER: Found ID []");
					$$ = TreeCreate(0,ARRAYDECL,$1,$3,NULL,NULL,NULL);
					}
			;

expr	:	expr '+' expr
			{	$$ = TreeCreate(0,ADD,"",0,$1,$3,NULL);
				}
		
		|	expr '-' expr
			{	$$ = TreeCreate(0,SUB,"",0,$1,$3,NULL);
				}
		
		|	expr '*' expr
			{	$$ = TreeCreate(0,MUL,"",0,$1,$3,NULL);
				}
		
		|	expr '/' expr
			{	$$ = TreeCreate(0,DIV,"",0,$1,$3,NULL);
				}
		
		|	'(' expr ')'
			{	$$ = $2;
				}
		
		|	DIGIT
			{	printf("\nPARSER: Found DIGIT\n");
				$$ = TreeCreate(0,NUM,"",$1,NULL,NULL,NULL);
				}
		
		|	ID
			{	printf("\nPARSER: Found ID");
				$$ = TreeCreate(0,IDFR,$1,0,NULL,NULL,NULL);
				}
		
		|	ID '[' expr ']'
			{
				printf("\nPARSER: Found ID []");
				$$ = TreeCreate(0,ARRAYIDFR,$1,0,$3,NULL,NULL);
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
