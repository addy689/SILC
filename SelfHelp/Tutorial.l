/*
	This file contains the Lex specification for obtaining POSTFIX notation of an expression
*/

%%
[\t ]+ ;


[0-9]+			{	strcpy(yylval.value,yytext);
					printf("\nThis is Dig Token %d by LEX - %s",ctr++,yylval.value);
					return DIGIT;
					}

[-+*\/\(\)\n]	{	printf("\nOp Token %c detected by LEX!",yytext[0]);
					return yytext[0];
					}

%%
int yyerror (void)
{
	return 1;
}
