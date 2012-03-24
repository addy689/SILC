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

%token READ WRITE BEGN END DECL ENDDECL INTEGER BOOLEAN WHILE DO ENDWHILE IF THEN ELSE ENDIF TRU FALS AND OR NOT
%type <T> declstmnt declist type identlist identname stlist statement elseStatement expr

%right '='
%left '+' '-'
%left '%'
%left '*' '/'
%left AND OR Eq NEq Gt Lt GEq LEq
%right NOT

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

statement	:	WHILE '(' expr ')' DO stlist ENDWHILE ';'
				{	printf("\nPARSER: Found WHILE DO ENDWHILE\n");
					$$ = TreeCreate(0,ITERATIVE,"",0,$3,$6,NULL);
					}
			
			|	IF '(' expr ')' THEN stlist elseStatement ENDIF ';'
				{	printf("\nPARSER: Found IF ELSE ENDIF\n");
					$$ = TreeCreate(0,CONDITIONAL,"",0,$3,$6,$7);
					}
			
			|	ID '=' expr ';'
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

elseStatement	:	ELSE stlist
					{	$$ = $2;
						}
			
				|	{	$$ = NULL;
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
			{	$$ = TreeCreate(INTGR,ADD,"",0,$1,$3,NULL);
				}
		
		|	expr '-' expr
			{	$$ = TreeCreate(INTGR,SUB,"",0,$1,$3,NULL);
				}
		
		|	expr '*' expr
			{	$$ = TreeCreate(INTGR,MUL,"",0,$1,$3,NULL);
				}
		
		|	expr '/' expr
			{	$$ = TreeCreate(INTGR,DIV,"",0,$1,$3,NULL);
				}
		
		|	expr '%' expr
			{	$$ = TreeCreate(INTGR,MOD,"",0,$1,$3,NULL);
				}
		
		|	expr Gt expr
			{	$$ = TreeCreate(BOOL,GT,"",0,$1,$3,NULL);
				}
		
		|	expr Lt expr
			{	$$ = TreeCreate(BOOL,LT,"",0,$1,$3,NULL);
				}
		
		|	expr GEq expr
			{	$$ = TreeCreate(BOOL,GTE,"",0,$1,$3,NULL);
				}
		
		|	expr LEq expr
			{	$$ = TreeCreate(BOOL,LTE,"",0,$1,$3,NULL);
				}
		
		|	expr Eq expr
			{	$$ = TreeCreate(BOOL,EQ,"",0,$1,$3,NULL);
				}
		
		|	expr NEq expr
			{	$$ = TreeCreate(BOOL,NE,"",0,$1,$3,NULL);
				}
		
		|	expr AND expr
			{	$$ = TreeCreate(BOOL,And,"",0,$1,$3,NULL);
				}
		
		|	expr OR expr
			{	$$ = TreeCreate(BOOL,Or,"",0,$1,$3,NULL);
				}
		
		|	NOT expr
			{	$$ = TreeCreate(BOOL,Not,"",0,$2,NULL,NULL);
				}
		
		|	TRU
			{	$$ = TreeCreate(BOOL,True,"",0,NULL,NULL,NULL);
				}
		
		|	FALS
			{	$$ = TreeCreate(BOOL,False,"",0,NULL,NULL,NULL);
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
