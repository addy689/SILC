/*
	This Lex file gives the desription an analyzer for detecting calculator tokens
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
