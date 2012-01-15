/*
	This file contains the Lex specification for implementing a simple calculator
*/

%{
	struct Tnode* allocateNode(int,int,char);
%}

%%
[\t ]+ ;

[0-9]+			{	yylval.T = allocateNode(atoi(yytext),0,'\0');
					return DIGIT;
					}

[-+*\/\(\)\n]		{	switch(yytext[0])
					{
						case '+':	yylval.T = allocateNode(0,ADD,'+');
									break;
						case '-':	yylval.T = allocateNode(0,SUB,'-');
									break;
						case '*':	yylval.T = allocateNode(0,MUL,'*');
									break;
						case '/':	yylval.T = allocateNode(0,DIV,'/');
									break;
						default :	;
					}
					
					return yytext[0];
					}

%%
struct Tnode* allocateNode(int value,int flg,char optr)
{
	struct Tnode *N;
	N = malloc(sizeof(struct Tnode));
	
	N->val = value;
	N->flag = flg;
	N->op = optr;
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
