/*
	This file contains the Lex specification for implementing a simple calculator
*/

%%
[\t ]+ ;

[0-9]+			{	yylval->T = malloc(sizeof(struct Tnode));
					yylval->T.val = atoi(yytext);
					yylval->T.flag = 0;
					(yylval->T)->l = (yylval->T)->r = NULL;
					return DIGIT;
					}

[-+*\/]			{	yylval->T = malloc(sizeof(struct Tnode));
					yylval->T.op = yytext[0];
					
					switch(yytext[0])
						{
							case '+':	yylval->T.flag = ADD;
										break;
							case '-':	yylval->T.flag = SUB;
										break;
							case '*':	yylval->T.flag = MUL;
										break;
							case '/':	yylval->T.flag = DIV;
										break;
						}

					(yylval->T)->l = (yylval->T)->r = NULL;
					return '+';
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
