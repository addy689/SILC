/*
	This Lex file gives the desription an analyzer for detecting a sequence of digits and adding 3 to it if its divisible by 7
*/

DIGIT	[0-9]

%%
{DIGIT}+	{
				int k;
				k = atoi(yytext);
				if(k%7==0) printf("%d",k+3);
				else printf("%d",k);
			}

%%
//Read Input File
int main(int argc, char *argv[])
{
	yyin=fopen(argv[1],"r");
	yylex();
	fclose(yyin);
	
	return 0;
}
