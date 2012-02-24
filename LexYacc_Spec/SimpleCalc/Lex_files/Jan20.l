/*
	This file contains the Lex specification for constructing an expression tree for a SIL program
*/

%%
[\t ]+	;

[\n][\t \n]+	{	return '\n';
					}

begin			{	return BEGN;
					}

end				{	return ED;
					}

read			{	printf("Token '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,RD,'\0',-1);
					return READ;
					}

write			{	printf("Token '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,WRT,'\0',-1);
					return WRITE;
					}

[a-z]			{	printf("\nAlpha token '%c' detected by LEX!",yytext[0]);
					yylval.T = allocateNode(0,VRBL,'\0',yytext[0]-'a');
					return VAR;
					}

[0-9]+			{	printf("\nNumber token '%c' detected by LEX!",yytext[0]);
					yylval.T = allocateNode(atoi(yytext),NUM,'\0',-1);
					return DIGIT;
					}

[-+*=/();\n]	{	printf("\nToken '%c' detected by LEX!",yytext[0]);
						switch(yytext[0])
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
	printf("\nAllocating\n");
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
