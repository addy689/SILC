/*
	This Lex file gives the desription an analyzer for detecting calculator tokens
*/

%%
[\t ]+ ;

[0-9]+			{	yylval=atoi(yytext);
					return DIGIT;
					}

[-+*\/\(\)\n]	{	return (int) yytext[0];
					}

. { yyerror("Unknown input"); }

%%
int yywrap(void)
{
  return 1;
}
