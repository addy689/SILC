/*
	This file contains the Yacc file specification for constructing an expression tree for a SIL program
*/

%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#define ADD 1
	#define SUB 2
	#define MUL 3
	#define DIV 4
	#define ASSGN 5
	#define NUM 6
	#define VRBL 7
	#define RD 8
	#define WRT 9
	#define STLIST 10
	
	char tmp[300]="";
%}

%union {	char c[300];
			}

%start prog
%token <c> DIGIT VR integer boolean DECL ENDDECL
%type <c> declbody decl identseq ident
%left <c> ',' '[' ']'

%%

prog	:	DECL '\n' declbody '\n' ENDDECL
			{	printf("\n\n--START--\n%s",$3);
				}

declbody	:	declbody '\n' decl
				{	tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,"\n");
					strcat(tmp,$3);
					strcpy($$,tmp);
					}
			
			|	decl
				{	strcpy($$,$1);
					}
			;

decl	:	integer identseq ';'
			{		printf("\nPARSER: Found decl: integer identseq");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,$2);
					strcat(tmp,";");
					strcpy($$,tmp);
					}
			
		|	boolean identseq ';'
			{		printf("\nPARSER: Found decl: boolean identseq");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,$2);
					strcat(tmp,";");
					strcpy($$,tmp);
					}
			
			;

identseq	:	identseq ',' ident
				{	
					printf("\nPARSER: Found identseq: identseq ident");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,",");
					strcat(tmp,$3);
					strcpy($$,tmp);
					}
			
			|	ident ',' ident
				{	printf("\nPARSER: Found identseq: ident ,");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,",");
					strcat(tmp,$3);
					strcpy($$,tmp);
					}
			;

ident		:	VR
				{	printf("\nPARSER: Found ident: VR");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcpy($$,tmp);
					}
			
			|	VR '[' DIGIT ']'
				{	printf("\nPARSER: Found ident: VR[]");
					tmp[0]='\0';
					strcat(tmp,$1);
					strcat(tmp,"[");
					strcat(tmp,$3);
					strcat(tmp,"]");
					strcpy($$,tmp);
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
