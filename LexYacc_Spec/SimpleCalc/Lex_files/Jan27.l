/*
	This file contains the Lex specification for constructing an expression tree for a SIL program
*/

digit	[0-9]
letter	[A-Za-z]

%%
[\n\t ]+	;

decl			return DECL;

enddecl			return ENDDECL;

integer			return INTEGER;

boolean			return BOOLEAN;

begin			return BEGN;

end				return END;

read			{	printf("Token '%s' detected by LEX!",yytext);
					return READ;
					}

write			{	printf("Token '%s' detected by LEX!",yytext);
					return WRITE;
					}

(_|{letter})(_|{letter}|{digit})*		{	printf("\nIdentifier '%s' detected by LEX!",yytext);
											yylval.id = malloc(sizeof(char) * (strlen(yytext)+1));
											strcpy(yylval.id,yytext);
											return ID;
											}

{digit}+		{	printf("\nNumber token '%c' detected by LEX!",yytext[0]);
					yylval.intval = atoi(yytext);
					return DIGIT;
					}

[-+*=/,()\[\];]	{	printf("\nToken '%c' detected by LEX!",yytext[0]);
					return yytext[0];
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
