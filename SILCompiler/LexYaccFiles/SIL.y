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
%type <T> declstmnt declist type identlist identname stlist statement elseStatement expr farglist farg fargidseq

%right '='
%left AND OR
%left Eq NEq
%left Gt Lt GEq LEq
%left '+' '-'
%left '*' '/' '%'
%right NOT

%%
prog	:	DECL declist ENDDECL BEGN stlist END
			{	compile($2,$5);
				}
		;

declist	:	declist declstmnt
			{	$$ = TreeCreate(0,CONTINUE,"",0,$1,$2,NULL,line);
				}
		
		|	declstmnt
			{	$$ = TreeCreate(0,CONTINUE,"",0,$1,NULL,NULL,line);
				}
		
		|	{	$$ = NULL;
				}
		
		;

declstmnt	:	type identlist ';'
				{	$$ = TreeCreate(0,DECLSTATEMENT,"",0,$1,$2,NULL,line);
					}
			
			;

stlist	:	stlist statement
			{	$$ = TreeCreate(0,CONTINUE,"",0,$1,$2,NULL,line);
				}
		
		|	statement
			{	$$ = TreeCreate(0,CONTINUE,"",0,$1,NULL,NULL,line);
				}
		
		|	{	$$ = NULL;
				}
		
		;

statement	:	WHILE '(' expr ')' DO stlist ENDWHILE ';'
				{	$$ = TreeCreate(0,ITERATIVE,"",0,$3,$6,NULL,$3->LINE);
					}
			
			|	IF '(' expr ')' THEN stlist elseStatement ENDIF ';'
				{	$$ = TreeCreate(0,CONDITIONAL,"",0,$3,$6,$7,$3->LINE);
					}
			
			|	ID '=' expr ';'
				{	$$ = TreeCreate(0,ASSIGN,$1,0,$3,NULL,NULL,$3->LINE);
					}
			
			|	ID '[' expr ']' '=' expr ';'
				{	$$ = TreeCreate(0,ARRAYASSIGN,$1,0,$3,$6,NULL,$3->LINE);
					}
			
			|	READ '(' ID ')' ';'
				{	$$ = TreeCreate(0,RD,$3,0,NULL,NULL,NULL,line);
					}
			
			|	READ '(' ID '[' expr ']' ')' ';'
				{	$$ = TreeCreate(0,ARRAYRD,$3,0,$5,NULL,NULL,$5->LINE);
					}
			
			|	WRITE '(' expr ')' ';'
				{	$$ = TreeCreate(0,WRIT,"",0,$3,NULL,NULL,$3->LINE);
					}
			
			;

elseStatement	:	ELSE stlist
					{	$$ = $2;
						}
			
				|	{	$$ = NULL;
						}
			
				;

identlist	:	identlist ',' identname
				{	$$ = TreeCreate(0,CONTINUE,"",0,$1,$3,NULL,line);
					}
			
			|	identname
				{	$$ = TreeCreate(0,CONTINUE,"",0,$1,NULL,NULL,line);
					}
			
			;

identname	:	ID
				{	$$ = TreeCreate(0,IDFRDECL,$1,0,NULL,NULL,NULL,line);
					}
			
			|	ID '[' DIGIT ']'
				{	$$ = TreeCreate(0,ARRAYDECL,$1,$3,NULL,NULL,NULL,line);
					}
			
			|	ID '(' farglist ')'
				{	
					}
			
			;

farglist	:	farglist ';' farg
				{	
					}
			
			|	farg
				{	
					}
			
			;

farg		:	type fargidseq
				{	
					}
		
			;

fargidseq	:	fargidseq ',' ID
				{	
					}
			
			|	fargidseq ',' ID '[' DIGIT ']'
				{	
					}
			
			|	ID
				{	
					}
			
			|	ID '[' DIGIT ']'
				{	
					}
			
			;

type	:	INTEGER
			{	$$ = TreeCreate(INTGR,DATATYPE,"",0,NULL,NULL,NULL,line);
				}
		
		|	BOOLEAN
			{	$$ = TreeCreate(BOOL,DATATYPE,"",0,NULL,NULL,NULL,line);
				}
		
		;

expr	:	expr '+' expr
			{	$$ = TreeCreate(INTGR,ADD,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr '-' expr
			{	$$ = TreeCreate(INTGR,SUB,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr '*' expr
			{	$$ = TreeCreate(INTGR,MUL,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr '/' expr
			{	$$ = TreeCreate(INTGR,DIV,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr '%' expr
			{	$$ = TreeCreate(INTGR,MOD,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr Gt expr
			{	$$ = TreeCreate(BOOL,GT,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr Lt expr
			{	$$ = TreeCreate(BOOL,LT,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr GEq expr
			{	$$ = TreeCreate(BOOL,GTE,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr LEq expr
			{	$$ = TreeCreate(BOOL,LTE,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr Eq expr
			{	$$ = TreeCreate(BOOL,EQ,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr NEq expr
			{	$$ = TreeCreate(BOOL,NE,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr AND expr
			{	$$ = TreeCreate(BOOL,And,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	expr OR expr
			{	$$ = TreeCreate(BOOL,Or,"",0,$1,$3,NULL,$1->LINE);
				}
		
		|	NOT expr
			{	$$ = TreeCreate(BOOL,Not,"",0,$2,NULL,NULL,$2->LINE);
				}
		
		|	TRU
			{	$$ = TreeCreate(BOOL,True,"",0,NULL,NULL,NULL,line);
				}
		
		|	FALS
			{	$$ = TreeCreate(BOOL,False,"",0,NULL,NULL,NULL,line);
				}
		
		|	'(' expr ')'
			{	$$ = $2;
				}
		
		|	DIGIT
			{	$$ = TreeCreate(0,NUM,"",$1,NULL,NULL,NULL,line);
				}
		
		|	ID
			{	$$ = TreeCreate(0,IDFR,$1,0,NULL,NULL,NULL,line);
				}
		
		|	ID '[' expr ']'
			{	$$ = TreeCreate(0,ARRAYIDFR,$1,0,$3,NULL,NULL,$3->LINE);
				}
		
		;

%%
#include "lex.yy.c"

int main(int argc,char *argv[])
{
	line = 1;
	yyin = fopen(argv[1],"r");
	yyparse();
	fclose(yyin);
	return 0;
}
