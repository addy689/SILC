/*
	This file contains the Lex specification for obtaining PREFIX notation of an expression (without computing the expression tree).
*/

%%
[\t ]+ ;

[0-9]+			{	strcpy(yylval.value,yytext);
					return DIGIT;
					}

[-+*\/\(\)\n]	{	return yytext[0];
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
