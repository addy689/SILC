/*
	This file contains the Lex file specification for implementing a simple calculator
*/

%%
[\t ]+ ;

[0-9]+			{	yylval = atoi(yytext);
					return DIGIT;
					}

[-+*\/\(\)\n]	{	return (int) yytext[0];
					}

. { yyerror("Unknown input"); }

%%
int yyerror (void)
{
	return 1;
}

int yywrap(void)
{
  return 1;
}
