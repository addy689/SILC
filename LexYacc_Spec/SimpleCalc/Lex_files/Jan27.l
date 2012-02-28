/*
	This file contains the Lex specification for constructing an expression tree for a SIL program
*/

digit	[0-9]
letter	[A-Za-z]

%%
[\t ]+	;

[\n][\t \n]+	return '\n';

decl			return DECL;

enddecl			return ENDDECL;

integer[ ]		return integer;

boolean[ ]		return boolean;

begin			return BEGN;

end				return ED;

read			{	printf("Token '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,RD,"",0,NULL,NULL,NULL);
					return READ;
					}

write			{	printf("Token '%s' detected by LEX!",yytext);
					yylval.T = allocateNode(0,WRT,"",0,NULL,NULL,NULL);
					return WRITE;
					}

(_|{letter})(_|{letter}|{digit})*		{	printf("\nIdentifier '%s' detected by LEX!",yytext);
											yylval.T = allocateNode(0,IDFR,yytext,0,NULL,NULL,NULL);
											return ID;
											}

{digit}+		{	printf("\nNumber token '%c' detected by LEX!",yytext[0]);
					yylval.T = allocateNode(0,NUM,"",atoi(yytext),NULL,NULL,NULL);
					return DIGIT;
					}

[-+*=/,()\[\];\n]	{	printf("\nToken '%c' detected by LEX!",yytext[0]);
						switch(yytext[0])
						{
							case '+':	yylval.T = allocateNode(0,ADD,"",0,NULL,NULL,NULL);
										break;
							case '-':	yylval.T = allocateNode(0,SUB,"",0,NULL,NULL,NULL);
										break;
							case '*':	yylval.T = allocateNode(0,MUL,"",0,NULL,NULL,NULL);
										break;
							case '/':	yylval.T = allocateNode(0,DIV,"",0,NULL,NULL,NULL);
										break;
							case '=':	yylval.T = allocateNode(0,ASSGN,"",0,NULL,NULL,NULL);
										break;
							default :	;
						}
						
						return yytext[0];
					}

%%
struct Tnode* allocateNode(int TYPE,int NODETYPE,char *NAME,int VALUE,struct Tnode *Ptr1,struct Tnode *Ptr2,struct Tnode *Ptr3)
{
	printf("\nAllocating\n");
	struct Tnode *N;
	N = malloc(sizeof(struct Tnode));
	
	N->TYPE = TYPE;
	N->NODETYPE = NODETYPE;
	
	N->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(N->NAME,NAME);
	N->VALUE = VALUE;
	
	N->Ptr1 = Ptr1;
	N->Ptr2 = Ptr2;
	N-> Ptr3 = Ptr3;
	
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
