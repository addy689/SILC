/*
	This file contains the Lex specification for implementing a simple calculator
*/

%%
[\t ]+ ;

[0-9]+			{	yylval = yytext;
					return DIGIT;
					}

[-+*\/\(\)\n]	{	return (int) yytext[0];
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
