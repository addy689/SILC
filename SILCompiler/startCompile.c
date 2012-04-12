/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "startCompile.h"


/*	Compile the source code using the constructed Abstract Syntax Tree 
	Arguments are the root nodes of -
	1) Global declaration AST,
	2) Function definitions AST
	3) main() function definition AST
	*/
void compile(Tnode *gdeclroot,Tnode *fdefroot,Tnode *mainroot)
{
	error = 0;
	
	locpos = -1;
	labelcnt = 0;
	globalInstall(gdeclroot);
	
	if(fdefroot != NULL)
	{
		funcSemanticCheck(fdefroot);
		checkFuncDecl(fdefroot->LINE);
		funcSemanticCheck(mainroot);
	}
	
	printf("\nRUN - \n");
	if(error == 0)
	{
		module = INTERPRET;
		Gallocate();
		funcroot = fdefroot;
		mroot = mainroot;
		
		struct Lsymbol *Ltable;
		Ltable = NULL;
		interpreter(mainroot,&Ltable);
		
/*		module = CODEGEN;*/
/*		fp = fopen("SIMCode","w");*/
/*		regcnt = 0;*/
/*		fprintf(fp,"START\n");*/
/*		funcCodeGen(fdefroot);*/
/*		funcCodeGen(mainroot);*/
/*		fprintf(fp,"HALT\n");*/
/*		fclose(fp);*/
	}
}
