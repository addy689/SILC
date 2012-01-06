/*
	This Lex file gives the desription an analyzer for detecting a sequence of spaces or tabs at the end of a line and discarding it; for any other spaces/tabs it prints a '+'
*/

%%
[ \t]+$		;
[ \t]+		printf("+");

%%
//Read Input File
int main(int argc, char *argv[])
{
	yyin=fopen(argv[1],"r");
	yylex();
	fclose(yyin);
	
	return 0;
}
