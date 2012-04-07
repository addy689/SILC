/*
	This file contains the Lex specification for constructing an expression tree for a SIL program
*/

digit	[0-9]
letter	[A-Za-z]

%%
[\n]	line++;

[\t ]+		;

main			return MAIN;

return			return RETURN;

decl			return DECL;

enddecl			return ENDDECL;

integer			return INTEGER;

boolean			return BOOLEAN;

begin			return BEGN;

end				return END;

while			return WHILE;

do				return DO;

endwhile		return ENDWHILE;

if				return IF;

then			return THEN;

else			return ELSE;

endif			return ENDIF;

read			return READ;

write			return WRITE;

"=="			return Eq;

"!="			return NEq;

"<"				return Lt;

">"				return Gt;

">="			return GEq;

"<="			return LEq;

AND				return AND;

OR				return OR;

NOT				return NOT;

TRUE			return TRU;

FALSE			return FALS;

(_|{letter})(_|{letter}|{digit})*		{	yylval.id = malloc(sizeof(char) * (strlen(yytext)+1));
											strcpy(yylval.id,yytext);
											return ID;
											}

{digit}+		{	yylval.intval = atoi(yytext);
					return CONST;
					}

[-+*=%/,()\[\];{}&]		{	return yytext[0];
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
