/*
	This file contains the Lex specification for constructing an expression tree for a SIL program
*/

digit	[0-9]
letter	[A-Za-z]

%%
[\t ]+	;

[\n][\t \n]+	return '\n';

if				{	printf("\nToken '%s' detected by LEX!",yytext);
					return IF; //allocate node here
					}

then			{	printf("\nToken '%s' detected by LEX!",yytext);
					return THEN; //allocate node here
					}

else			{	printf("\nToken '%s' detected by LEX!",yytext);
					return ELSE; //allocate node here
					}

endif			{	printf("\nToken '%s' detected by LEX!",yytext);
					return ENDIF; //allocate node here
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

boolean[ ]			{	strcpy(yylval.c,yytext);
						return boolean;  //allocate node here
						}

begin			{	return BEGN;
					}

end				{	return ED;
					}

read			{	printf("Token '%s' detected by LEX!",yytext);
					strcpy(yylval.c,yytext);
					return READ;
					}

write			{	printf("Token '%s' detected by LEX!",yytext);
					strcpy(yylval.c,yytext);
					return WRITE;
					}

(_|{letter})(_|{letter}|{digit})*		{	printf("\nIdentifier '%s' detected by LEX!",yytext);
												strcpy(yylval.c,yytext);
												return ID;
												}

{digit}+		{	printf("\nNumber token '%s' detected by LEX!",yytext);
					strcpy(yylval.c,yytext);
					return DIGIT;
					}

[-+*=/()\;,\[\]\n<>]		{	printf("\nToken '%s' detected by LEX!",yytext);
						return *yytext;
					}

%%

int yyerror (void)
{
	return 1;
}

int yywrap(void)
{
  return 1;
}
