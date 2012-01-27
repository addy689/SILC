/*
	This file contains the Lex specification for implementing a simple calculator
*/

%%
[\t ]+	;

BEGIN			{	return begin;
					}

END				{	return end;
					}

[a-z]			{	yylval.T = allocateNode(0,VRBL,'\0',yytext[0]-'a');
					
					printf("\nYeah baby");
					
					return VAR;
					}

read			{	yylval.T = allocateNode(0,RD,'\0',-1);
					return READ;
					}

[0-9]+			{	yylval.T = allocateNode(atoi(yytext),NUM,'\0',-1);
					return DIGIT;
					}

[-+*\/\(\)\n\;\=]	{	switch(yytext[0])
						{
							case '+':	yylval.T = allocateNode(0,ADD,'+',-1);
										break;
							case '-':	yylval.T = allocateNode(0,SUB,'-',-1);
										break;
							case '*':	yylval.T = allocateNode(0,MUL,'*',-1);
										break;
							case '/':	yylval.T = allocateNode(0,DIV,'/',-1);
										break;
							case '=':	yylval.T = allocateNode(0,ASSGN,'=',-1);
										break;
							default :	;
						}
						
						return yytext[0];
						}

%%
struct Tnode* allocateNode(int value,int flg,char optr,int binding)
{
	printf("\nAllocating");
	struct Tnode *N;
	N = malloc(sizeof(struct Tnode));
	
	N->val = value;
	N->flag = flg;
	N->op = optr;
	N->binding = binding;
	N->l = N->r = NULL;
	
	return N;
}

int yyerror (void)
{
	return 1;
}

int yywrap(void)
{
  return 1;
}
