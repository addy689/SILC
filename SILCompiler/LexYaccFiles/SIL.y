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

%start Prog
%token <intval> CONST
%token <id> ID

%token MAIN DECL ENDDECL RETURN READ WRITE BEGN END INTEGER BOOLEAN WHILE DO ENDWHILE IF THEN ELSE ENDIF TRU FALS
%type <T> GDefblock GDefList GDecl GIdList GId
%type <T> FdefList Fdef Mainblock RetStmt
%type <T> LDefblock LDefList LDecl LIdList LId
%type <T> ArgList ArgDecl ArgIdList ArgId Type Body StmtList Stmt ElseStmt Expr ExprList

%left ','
%right '='
%left AND OR
%left Eq NEq
%left Gt Lt GEq LEq
%left '+' '-'
%left '*' '/' '%'
%right NOT
%left '(' ')' '[' ']'

%%
Prog		:	GDefblock FdefList Mainblock
				{	printf("DONE Baby");
					compile($1,$2,$3);
					}
			
			;

GDefblock	:	DECL GDefList ENDDECL
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$2,NULL,NULL,line);
					}
			
			;

GDefList	:	GDefList GDecl
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$2,NULL,line);
					}
			
			|	GDecl
				{	$$ = $1;
					}
			
			|	{	$$ = NULL;
					}
			
			;

GDecl		:	Type GIdList ';'
				{	$$ = TreeCreate(0,DECLSTATEMENT,"",0,NULL,$1,$2,NULL,line);
					}
			
			;

GIdList		:	GIdList ',' GId
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$3,NULL,line);
					}
			
			|	GId
				{	$$ = $1;
					}
			
			;

GId			:	ID
				{	$$ = TreeCreate(0,IDFRDECL,$1,0,NULL,NULL,NULL,NULL,line);
					}
			
			|	ID '[' CONST ']'
					{	$$ = TreeCreate(0,ARRAYDECL,$1,$3,NULL,NULL,NULL,NULL,line);
						}
			
			|	ID '(' ArgList ')'
				{	$$ = TreeCreate(0,FUNCDECL,$1,0,$3,NULL,NULL,NULL,line);
					}
			
			;

FdefList	:	FdefList Fdef
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$2,NULL,line);
					}
			
			|	Fdef
				{	$$ = $1;
					}
			
			;

Fdef		:	Type ID '(' ArgList ')' '{' LDefblock Body '}'
				{	$$ = TreeCreate(0,FUNCBLOCK,$2,-1,$4,$1,$7,$8,$1->LINE);
					}
			;

Mainblock	:	INTEGER MAIN '(' ')' '{' LDefblock Body '}'
				{	$$ = TreeCreate(0,MAINBLOCK,"",0,NULL,$6,$7,NULL,line);
					}
			;

LDefblock	:	DECL LDefList ENDDECL
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$2,NULL,NULL,line);
					}
			
			;

LDefList	:	LDefList LDecl
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$2,NULL,line);
					}
			
			|	LDecl
				{	$$ = $1;
					}
			
			|	{	$$ = NULL;
					}
			
			;

LDecl		:	Type LIdList ';'
				{	$$ = TreeCreate(0,DECLSTATEMENT,"",0,NULL,$1,$2,NULL,$2->LINE);
					}
			
			;

LIdList		:	LIdList ',' LId
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$3,NULL,line);
					}
			
			|	LId
				{	$$ = $1;
					}
			
			;

LId			:	ID
				{	$$ = TreeCreate(0,IDFRDECL,$1,0,NULL,NULL,NULL,NULL,line);
					}
			
			;

ArgList		:	ArgList ';' ArgDecl
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	ArgDecl
				{	$$ = $1;
					}
			
			|	{	$$ = NULL;
					}
			
			;

ArgDecl		:	Type ArgIdList
				{	$$ = TreeCreate(0,ARGSTATEMENT,"",0,NULL,$1,$2,NULL,$2->LINE);
					}
			
			;

ArgIdList	:	ArgIdList ',' ArgId
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	ArgId
				{	$$ = $1;
					}
			
			;

ArgId		:	ID
				{	$$ = TreeCreate(0,IDARG,$1,0,NULL,NULL,NULL,NULL,line);
					}
			
			;

Type		:	INTEGER
				{	$$ = TreeCreate(INTGR,DATATYPE,"",0,NULL,NULL,NULL,NULL,line);
					}
			
			|	BOOLEAN
				{	$$ = TreeCreate(BOOL,DATATYPE,"",0,NULL,NULL,NULL,NULL,line);
					}
			
			;

Body		:	BEGN StmtList RetStmt END
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$2,NULL,NULL,line);
					}
			
			;

RetStmt		:	RETURN Expr ';'
				{	$$ = TreeCreate(0,RET,"",0,NULL,$2,NULL,NULL,$2->LINE);
					}
			;

StmtList	:	StmtList Stmt
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$2,NULL,line);
					}
			
			|	Stmt
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,NULL,NULL,line);
					}
			
			|	{	$$ = NULL;
					}
			
			;

Stmt		:	WHILE '(' Expr ')' DO StmtList ENDWHILE ';'
				{	$$ = TreeCreate(0,ITERATIVE,"",0,NULL,$3,$6,NULL,$3->LINE);
					}
			
			|	IF '(' Expr ')' THEN StmtList ElseStmt ENDIF ';'
				{	$$ = TreeCreate(0,CONDITIONAL,"",0,NULL,$3,$6,$7,$3->LINE);
					}
			
			|	ID '=' Expr ';'
				{	$$ = TreeCreate(0,ASSIGN,$1,0,NULL,$3,NULL,NULL,$3->LINE);
					}
			
			|	ID '[' Expr ']' '=' Expr ';'
				{	$$ = TreeCreate(0,ARRAYASSIGN,$1,0,NULL,$3,$6,NULL,$3->LINE);
					}
			
			|	READ '(' ID ')' ';'
				{	$$ = TreeCreate(0,RD,$3,0,NULL,NULL,NULL,NULL,line);
					}
			
			|	READ '(' ID '[' Expr ']' ')' ';'
				{	$$ = TreeCreate(0,ARRAYRD,$3,0,NULL,$5,NULL,NULL,$5->LINE);
					}
			
			|	WRITE '(' Expr ')' ';'
				{	$$ = TreeCreate(0,WRIT,"",0,NULL,$3,NULL,NULL,$3->LINE);
					}
			
			;

ElseStmt	:	ELSE StmtList
					{	$$ = $2;
						}
			
			|	{	$$ = NULL;
					}
			
			;

Expr		:	Expr '+' Expr
				{	$$ = TreeCreate(INTGR,ADD,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr '-' Expr
				{	$$ = TreeCreate(INTGR,SUB,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr '*' Expr
				{	$$ = TreeCreate(INTGR,MUL,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr '/' Expr
				{	$$ = TreeCreate(INTGR,DIV,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr '%' Expr
				{	$$ = TreeCreate(INTGR,MOD,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr Gt Expr
				{	$$ = TreeCreate(BOOL,GT,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr Lt Expr
				{	$$ = TreeCreate(BOOL,LT,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr GEq Expr
				{	$$ = TreeCreate(BOOL,GTE,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr LEq Expr
				{	$$ = TreeCreate(BOOL,LTE,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr Eq Expr
				{	$$ = TreeCreate(BOOL,EQ,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr NEq Expr
				{	$$ = TreeCreate(BOOL,NE,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr AND Expr
				{	$$ = TreeCreate(BOOL,And,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr OR Expr
				{	$$ = TreeCreate(BOOL,Or,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	NOT Expr
				{	$$ = TreeCreate(BOOL,Not,"",0,NULL,$2,NULL,NULL,$2->LINE);
					}
			
			|	TRU
				{	$$ = TreeCreate(BOOL,True,"",0,NULL,NULL,NULL,NULL,line);
					}
			
			|	FALS
				{	$$ = TreeCreate(BOOL,False,"",0,NULL,NULL,NULL,NULL,line);
					}
			
			|	'(' Expr ')'
				{	$$ = $2;
					}
			
			|	CONST
				{	$$ = TreeCreate(0,NUM,"",$1,NULL,NULL,NULL,NULL,line);
					}
			
			|	ID
				{	$$ = TreeCreate(0,IDFR,$1,0,NULL,NULL,NULL,NULL,line);
					}
			
			|	ID '[' Expr ']'
				{	$$ = TreeCreate(0,ARRAYIDFR,$1,0,NULL,$3,NULL,NULL,$3->LINE);
					}
			
			|	ID '(' ExprList ')'
				{	$$ = TreeCreate(0,FUNCCALL,$1,0,NULL,$3,NULL,NULL,$3->LINE);
					}
			
			;

ExprList	:	ExprList ',' Expr
				{	$$ = TreeCreate(0,CONTINUE,"",0,NULL,$1,$3,NULL,$1->LINE);
					}
			
			|	Expr
				{	$$ = $1;
					}
			
			|	{	$$ = NULL;
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
