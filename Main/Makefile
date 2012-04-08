sil.out: SIL.tab.c compilerLib.o
	gcc $(CFLAGS) -o sil.out SIL.tab.c -ll compilerLib.o

compilerLib.o: ../SILCompiler/compilerLib.c ../SILCompiler/compilerLib.h
	gcc $(CFLAGS) -c ../SILCompiler/compilerLib.c

SIL.tab.c: ../SILCompiler/LexYaccFiles/SIL.y lex.yy.c
	bison ../SILCompiler/LexYaccFiles/SIL.y

lex.yy.c: ../SILCompiler/LexYaccFiles/SIL.l
	flex ../SILCompiler/LexYaccFiles/SIL.l

clean: 
	rm SIL.tab.c
	rm lex.yy.c
	rm *.o
	rm sil.out
