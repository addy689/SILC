/*
	This file contains the Lex specification for constructing an expression tree for a SIL program
*/

digit	[0-9]
letter	[A-Za-z]

%%
[\t ]+	;

[\n][\t \n]+	{	return '\n';
					}

decl			{	printf("\nToken '%s' detected by LEX!",yytext);
					return DECL; //allocate node here
					}

enddecl			{	printf("\nToken '%s' detected by LEX!",yytext);
					return ENDDECL; //allocate node here
					}

integer[ ]		{	printf("\nToken '%s' detected by LEX!",yytext);
					strcpy(yylval.c,yytext);
					return integer;  //allocate node here
					}

boolean[ ]		{	strcpy(yylval.c,yytext);
					return boolean;  //allocate node here
					}

begin			{	return BEGN;
					}

end				{	return ED;
					}

read			{	printf("Token '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,RD,'\0','\0');
					return READ;
					}

write			{	printf("Token '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,WRT,'\0','\0');
					return WRITE;
					}

(_|{letter})(_|{letter}|{digit})*		{	printf("\nIdentifier '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,VRBL,'\0',yytext);
					return VAR;
					}

{digit}+		{	printf("\nNumber token '%c' detected by LEX!",yytext[0]);
					yylval.T = allocateNode(atoi(yytext),NUM,'\0','\0');
					return DIGIT;
					}

[-+*=/,()\[\];\n]	{	printf("\nToken '%c' detected by LEX!",yytext[0]);
						switch(yytext[0])
						{
							case '+':	yylval.T = allocateNode(0,ADD,'+','\0');
										break;
							case '-':	yylval.T = allocateNode(0,SUB,'-','\0');
										break;
							case '*':	yylval.T = allocateNode(0,MUL,'*','\0');
										break;
							case '/':	yylval.T = allocateNode(0,DIV,'/','\0');
										break;
							case '=':	yylval.T = allocateNode(0,ASSGN,'=','\0');
										break;
							default :	;
						}
						
						return yytext[0];
					}

%%
struct Tnode* allocateNode(int value,int flg,char optr,char *name)
{
	printf("\nAllocating\n");
	struct Tnode *N;
	N = malloc(sizeof(struct Tnode));
	
	if(flg==VRBL)
	{
		N->Gentry = malloc(sizeof(struct Gsymbol));
		strcpy(N->Gentry->NAME,name);
	}
	
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
