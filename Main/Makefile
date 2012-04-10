all:	sil

sil: SIL.tab.c startCompile.o compilerLib.o semanticChecks.o interpret.o codeGen.o
	gcc $(CFLAGS) -o sil SIL.tab.c -ll *.o

startCompile.o: ../SILCompiler/startCompile.c
		gcc $(CFLAGS) -c ../SILCompiler/startCompile.c

codeGen.o: ../SILCompiler/codeGen.c
	gcc $(CFLAGS) -c ../SILCompiler/codeGen.c

interpret.o: ../SILCompiler/interpret.c
	gcc $(CFLAGS) -c ../SILCompiler/interpret.c

semanticChecks.o: ../SILCompiler/semanticChecks.c
	gcc $(CFLAGS) -c ../SILCompiler/semanticChecks.c

compilerLib.o: ../SILCompiler/compilerLib.c
	gcc $(CFLAGS) -c ../SILCompiler/compilerLib.c

SIL.tab.c: ../SILCompiler/LexYaccFiles/SIL.y lex.yy.c
	bison ../SILCompiler/LexYaccFiles/SIL.y

lex.yy.c: ../SILCompiler/LexYaccFiles/SIL.l
	flex ../SILCompiler/LexYaccFiles/SIL.l

clean: 
	rm SIL.tab.c
	rm lex.yy.c
	rm *.o
	rm sil
