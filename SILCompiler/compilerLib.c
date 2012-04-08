/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "compilerLib.h"

/*	Create a node for Abstract Syntax Tree
	*/
Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *ArgList,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3,int LINE)
{
	Tnode *N = malloc(sizeof(Tnode));
	
	N->TYPE = TYPE;
	N->NODETYPE = NODETYPE;
	
	N->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(N->NAME,NAME);
	N->VALUE = VALUE;
	
	N->ArgList = ArgList;
	N->Ptr1 = Ptr1;
	N->Ptr2 = Ptr2;
	N->Ptr3 = Ptr3;
	
	N->LINE = LINE;
	
	return N;
}


/*	Compile the source code using the constructed Abstract Syntax Tree 
	Arguments are the root nodes of -
	1) Global declaration AST,
	2) Function definitions AST
	3) main() function definition AST
	*/
void compile(Tnode *gdeclroot,Tnode *fdefroot,Tnode *mainroot)
{
	error = 0;
	loc = 0;

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
		Gallocate();
		funcroot = fdefroot;
		mroot = mainroot;
		
		struct Lsymbol *Ltable;
		Ltable = NULL;
		interpreter(mainroot,&Ltable);
		
		fp = fopen("SIMCode","w");
		regcnt = 0;
		labelcnt = -1;
		codeGenerate(mainroot->Ptr2);
		fclose(fp);
	}
}


/*	Install all global declarations (identifiers,functions,arrays)
	present in source program into a global symbol table
	*/
void globalInstall(Tnode *root)
{
	struct Gsymbol *gnode;
	ArgStruct *arg;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	globalInstall(root->Ptr1);
								globalInstall(root->Ptr2);
								break;
		
		case DECLSTATEMENT	:	decnode = root->Ptr1;
								globalInstall(root->Ptr2);
								break;
		
		case IDFRDECL		:
		
		case ARRAYDECL		:
		
		case FUNCDECL		:	gnode = Glookup(root->NAME);
								
								if(gnode != NULL)
								{
									if(root->TYPE != gnode->TYPE)
										printf("\nSIL:%d: Error: conflicting types for '%s'",root->LINE,root->NAME);
									else
										printf("\nSIL:%d: Error: redeclaration of '%s'",root->LINE,root->NAME);
									
									error++;
								}
								
								globalInstall(root->ArgList);
								
								if(gnode == NULL)
									Ginstall(root->NAME,decnode->TYPE,root->VALUE,Arghead);
								
								break;
		
		case ARGSTATEMENT	:	argnode = root->Ptr1;
								entry = 1;
								globalInstall(root->Ptr2);
								break;
		
		case IDALIASARG		:
		
		case IDARG			:	arg = argLookup(root->NAME,Arghead);
								
								if(arg != NULL)
								{
									if(root->TYPE != arg->TYPE)
										printf("\nSIL:%d: Error: conflicting types for '%s'",root->LINE,root->NAME);
									else
										printf("\nSIL:%d: Error: redefinition of parameter '%s'",root->LINE,root->NAME);
									
									error++;
								}
								
								else argInstall(root->NAME,argnode->TYPE);
								
								break;
	}
}


/*	Install function arguments present in global declaration of function into an argument list
	*/
void argInstall(char *NAME,int TYPE)
{
	ArgStruct *Anode = malloc(sizeof(ArgStruct));
	
	Anode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Anode->NAME,NAME);
	
	Anode->TYPE = TYPE;
	Anode->FLAG = 0;
	
	if(entry == 1)
	{
		Anode->NEXT = Arghead;
		Arghead = Anode;
	}
	
	else
	{
		Anode->NEXT = Argrear->NEXT;
		Argrear->NEXT = Anode;
	}
	
	Argrear = Anode;
	entry++;
}


/*	Check a function's definition for semantic errors
	*/
int funcSemanticCheck(Tnode *root)
{
	ArgStruct *arg;
	struct Lsymbol *Lhead;
	
	Lhead = NULL;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	funcSemanticCheck(root->Ptr1);
								funcSemanticCheck(root->Ptr2);
								return;
		
		case FUNCBLOCK		:	argLocalInstall(root->ArgList,&Lhead);
								
								gtemp = Glookup(root->NAME);
								functype = root->Ptr1->TYPE;
								
								funcTypeCheck(root);
								
								if(gtemp)
								{
									funcArgCheck(root->ArgList);
									
									arg = gtemp->ARGLIST;
									
									while(arg)
									{
										if(arg->FLAG == 0)
										{
											printf("\nSIL:%d: Error: parameter '%s' has been defined in global declaration of function '%s', but is missing in definition",root->ArgList->LINE,arg->NAME,gtemp->NAME);
											error++;
										}
										arg = arg->NEXT;
									}
								}
								
								localInstall(root->Ptr2,&Lhead);
								bodySemanticCheck(root->Ptr3,&Lhead);
								
								return;
		
		case MAINBLOCK		:	functype = root->TYPE;
								localInstall(root->Ptr1,&Lhead);
								bodySemanticCheck(root->Ptr2,&Lhead);
								
								return;
	}
}


/*	Installs function arguments into that function's local symbol table (with semantic checking)
	*/
void argLocalInstall(Tnode *root,struct Lsymbol **Lhead)
{
	struct Lsymbol *lnode;
	ArgStruct *arg;
	char typ[10];
	
	if(root == NULL)
		return;

	switch(root->NODETYPE)
	{
		case CONTINUE		:	argLocalInstall(root->Ptr1,Lhead);
								argLocalInstall(root->Ptr2,Lhead);
								break;
		
		case ARGSTATEMENT	:	argnode = root->Ptr1;
								argLocalInstall(root->Ptr2,Lhead);
								break;
		
		case IDALIASARG		:
		
		case IDARG			:	lnode = Llookup(root->NAME,Lhead);
								
								if(lnode != NULL)
								{
									if(argnode->TYPE != lnode->TYPE)
										printf("\nSIL:%d: Error: conflicting types for '%s'",root->LINE,root->NAME);
									else
										printf("\nSIL:%d: Error: redefinition of parameter '%s'",root->LINE,root->NAME);
									
									error++;
								}
								
								else Linstall(root->NAME,argnode->TYPE,Lhead);
								
								break;
	}
}


/*	Lookup a function argument in the function argument list
	*/
ArgStruct *argLookup(char *NAME,ArgStruct *HEAD)
{
	ArgStruct *ptr = HEAD;
	
	while(ptr)
	{
		if(!strcmp(ptr->NAME,NAME))
			return ptr;
		
		ptr = ptr->NEXT;
	}
	
	return NULL;
}


/*	Check if function has
	1) A global declaration
	2) Same return type as mentioned in global declaration
	3) No redefinitions
	*/
int funcTypeCheck(Tnode *root)
{
	char typ[10];
	
	if(root == NULL)
		return;
	
	if(gtemp == NULL)
	{
		printf("\nSIL:%d: Error: function '%s' does not have a global declaration",root->LINE,root->NAME);
		error++;
	}
	
	else if(gtemp->SIZE == -1)
	{
		if(functype != gtemp->TYPE)
		{
			if(gtemp->TYPE == INTGR)
				strcpy(typ,"integer");
			else if(gtemp->TYPE == BOOL)
				strcpy(typ,"boolean");
			
			printf("\nSIL:%d: Error: function '%s' return type has been declared as '%s' in its global declaration",root->LINE,root->NAME,typ);
			error++;
		}
		
		gtemp->SIZE = -2;
	}
	
	else if(gtemp->SIZE == -2)
	{
		if(functype != gtemp->TYPE)
			printf("\nSIL:%d: Error: conflicting types for function '%s'",root->LINE,root->NAME);
		else
			printf("\nSIL:%d: Error: redefinition of function '%s'",root->LINE,root->NAME);
		
		error++;
	}
	
	return;
}


/*	Name and Type checking of function arguments against the global declaration
	*/
int funcArgCheck(Tnode *root)
{
	ArgStruct *arg;
	char typ[10];
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	funcArgCheck(root->Ptr1);
								funcArgCheck(root->Ptr2);
								return;
		
		
		case ARGSTATEMENT	:	argnode = root->Ptr1;
								funcArgCheck(root->Ptr2);
								return;
		
		case IDALIASARG		:
		
		case IDARG			:	arg = argLookup(root->NAME,gtemp->ARGLIST);
								
								if(arg == NULL)
								{
									printf("\nSIL:%d: Error: parameter '%s' not declared in global declaration of function '%s'",root->LINE,root->NAME,gtemp->NAME);
									error++;
								}
								
								else if(arg->FLAG == 0)
								{
									if(argnode->TYPE != arg->TYPE)
									{
										if(arg->TYPE == INTGR)
											strcpy(typ,"integer");
										else if(arg->TYPE == BOOL)
											strcpy(typ,"boolean");
										
										printf("\nSIL:%d: Error: parameter '%s' has type '%s' in global declaration of function '%s'",root->LINE,root->NAME,typ,gtemp->NAME);
										
										error++;
									}
									
									arg->FLAG = 1;
								}
								
								return;
	}
}


/*	Check for globally declared functions that do not have a function definition
	*/
void checkFuncDecl(int LINE)
{
	struct Gsymbol *ptr;
	
	ptr = Ghead;
	
	while(ptr)
	{
		if(ptr->SIZE == -1)
		{
			printf("\nSIL:%d: Error: function '%s' has been declared, but has no definition",LINE,ptr->NAME);
			error++;
		}
		
		ptr = ptr->NEXT;
	}
}


/*	Install all local declarations (identifiers) present in
	a function into that function's local symbol table
	*/
void localInstall(Tnode *root,struct Lsymbol **Lhead)
{
	struct Lsymbol *lnode;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	localInstall(root->Ptr1,Lhead);
								localInstall(root->Ptr2,Lhead);
								break;
		
		case DECLSTATEMENT	:	decnode = root->Ptr1;
								localInstall(root->Ptr2,Lhead);
								break;
		
		case IDFRDECL		:	lnode = Llookup(root->NAME,Lhead);
								
								if(lnode != NULL)
								{
									if(root->TYPE != lnode->TYPE)
										printf("\nSIL:%d: Error: conflicting types for '%s'",root->LINE,root->NAME);
									else
										printf("\nSIL:%d: Error: redeclaration of '%s'",root->LINE,root->NAME);
									
									error++;
								}
								
								else Linstall(root->NAME,decnode->TYPE,Lhead);
								
								break;
	}
}


/*	Check function body for the following-
	1) correct return type
	2) if a function is called,
		a) function that is called must have a global declaration
		b) call parameters must have same type as the arguments in function declaration
		c) number of call parameters must be equal to number of arguments in function declaration
	3) if, while condition must be a logical expression
	4) type checking during assignment operation
	5) read/write operations must not be allowed on boolean variables
	6) +,-,*,/,%,>,>=,<,<=,==,!= operations must not be allowed on logical expressions
	7) AND, OR and NOT operations must not be allowed on arithmetic expressions
	8) all identifiers must have either a local or global declaration
	*/
int bodySemanticCheck(Tnode *root,struct Lsymbol **Lhead)
{
	int t1,t2;
	char typ1[10],typ2[10];
	struct Lsymbol *lnode;
	struct Gsymbol *gnode;
	static struct Gsymbol *gfunc;
	static ArgStruct *Ahead;
	static int argcnt,paramcnt;
	
	if(root == NULL)
		return 0;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	bodySemanticCheck(root->Ptr1,Lhead);
								bodySemanticCheck(root->Ptr2,Lhead);
								return;
		
		case RET			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if(functype == INTGR)
									strcpy(typ1,"integer");
								else if(functype == BOOL)
									strcpy(typ1,"boolean");
								
								if(t1 == INTGR)
									strcpy(typ2,"integer");
								else if(t1 == BOOL)
									strcpy(typ2,"boolean");
								
								if(functype != t1)
								{
									printf("\nSIL:%d: Error: incompatible types when returning type '%s' but '%s' was expected",root->LINE,typ2,typ1);
									error++;
								}
								
								return;
		
		case FUNCCALL		:	gfunc = Glookup(root->NAME);
								
								if((gfunc == NULL) || (gfunc->SIZE == -1))
								{
									printf("\nSIL:%d: Error: undefined reference to '%s'",root->LINE,root->NAME);
									error++;
								}
								
								Ahead = gfunc->ARGLIST;
								argcnt = 0;
								paramcnt = 0;
								
								t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if(Ahead != NULL)
								{
									printf("\nSIL:%d: Error: too few arguments to function '%s'",root->LINE,gfunc->NAME);
									error++;
								}
								
								else if(argcnt < paramcnt)
								{
									printf("\nSIL:%d: Error: too many arguments to function '%s'",root->LINE,gfunc->NAME);
									error++;
								}
								
								return gtemp->TYPE;
		
		case FUNCPARAM		:	paramcnt++;
		
								t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if(Ahead == NULL)
									return;
								
								if(t1 != Ahead->TYPE)
								{
									if(Ahead->TYPE == INTGR)
										strcpy(typ1,"integer");
									else if(Ahead->TYPE == BOOL)
										strcpy(typ1,"boolean");
									
									printf("\nSIL:%d: Error: incompatible type for argument %d of '%s'; expected argument of type '%s'",root->LINE,argcnt+1,gfunc->NAME,typ1);
									
									error++;
								}
								
								argcnt++;
								Ahead = Ahead->NEXT;
								
								return 0;
		
		case ITERATIVE		:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if((t1 != BOOL) && (t1 != 0))
								{
									printf("\nSIL:%d: Error: while condition is not a logical expression",root->LINE);
									error++;
								}
								
								bodySemanticCheck(root->Ptr2,Lhead);
								return;

		case CONDITIONAL	:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if((t1 != BOOL) && (t1 != 0))
								{
									printf("\nSIL:%d: Error: if condition is not a logical expression",root->LINE);
									error++;
								}
								
								bodySemanticCheck(root->Ptr2,Lhead);
								bodySemanticCheck(root->Ptr3,Lhead);
								return;
		
		case ASSIGN			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if((lnode == NULL) && (gnode == NULL))
								{
									printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
									error++;
								}
								
								t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if(lnode != NULL)
									checkLocalAssign(root,lnode,t1);
								
								else if(gnode != NULL)
								{
									checkGlobalAssign(root,gnode,t1);
									if(((t1 == gnode->TYPE) || (t1 == 0)) && (gnode->SIZE>0))
									{
										printf("\nSIL:%d: Error: incompatible types when assigning to type '%s[%d]' from type '%s' variable",root->LINE,typ1,gnode->SIZE,typ2);
										error++;
									}
								}
								
								return 0;
		
		case ARRAYASSIGN	:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if((lnode == NULL) && (gnode == NULL))
								{
									printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
									error++;
								}
								
								t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if(t1 != 0)
								{
									if((t1 != INTGR))
									{
										printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
										error++;
									}
								
									if((lnode != NULL) || (gnode->SIZE == 0))
									{
										printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
										error ++;
									}
								}
								
								t1 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if(lnode != NULL)
									checkLocalAssign(root,lnode,t1);
								
								else if(gnode != NULL)
									checkGlobalAssign(root,gnode,t1);
								
								return 0;
		
		case RD				:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if((lnode == NULL) && (gnode == NULL))
								{
									printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
									error++;
								}
								
								else
								{
									if(lnode != NULL)
										t1 = (lnode->TYPE == BOOL);
									else if(gnode !=NULL)
										t1 = (gnode->TYPE == BOOL);
									
									if(t1)
									{
										printf("\nSIL:%d: Error: read operation is not allowed on boolean variables",root->LINE);
										error++;
									}
								}
								
								return 0;
		
		case ARRAYRD		:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if((lnode == NULL) && (gnode == NULL))
								{
									printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
									error++;
								}
								
								else if(((lnode != NULL) && (lnode->TYPE == BOOL)) || ((gnode != NULL) && (gnode->TYPE == BOOL)))
								{
									printf("\nSIL:%d: Error: read operation is not allowed on boolean variables",root->LINE);
									error++;
								}
								
								else
								{
									t1 = bodySemanticCheck(root->Ptr1,Lhead);
									
									if(t1 != 0)
									{
										if((t1 != INTGR))
										{
											printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
											error++;
										}
									
										if((lnode != NULL) || (gnode->SIZE == 0))
										{
											printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
											error ++;
										}
									}
								}
								
								return 0;
		
		case WRIT			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if((t1 != INTGR) && (t1 != 0))
								{
									printf("\nSIL:%d: Error: write operation not allowed on boolean variables",root->LINE);
									error++;
								}
								
								return 0;
		
		case DIV			:	if((root->Ptr2->VALUE == 0) && (root->Ptr2->NODETYPE == NUM))
									printf("\nSIL:%d: Warning: division by zero",root->LINE);
								
								t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
							
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: / does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case ADD			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: + does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case SUB			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: - does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case MUL			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: * does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case MOD			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: %% does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case GT				:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: > does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case LT				:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: < does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case GTE			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: >= does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case LTE			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: <= does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case EQ				:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: == does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case NE				:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == INTGR) && (t2 == INTGR))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: != does not support logical expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case And			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == BOOL) && (t2 == BOOL))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: AND does not support arithmetic expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case Or				:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								t2 = bodySemanticCheck(root->Ptr2,Lhead);
								
								if((t1 == BOOL) && (t2 == BOOL))
									return root->TYPE;
								
								else if((t1 != 0) && (t2 != 0))
								{
									printf("\nSIL:%d: Error: OR does not support arithmetic expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case Not			:	t1 = bodySemanticCheck(root->Ptr1,Lhead);
								
								if(t1 == BOOL)
									return root->TYPE;
								
								else if(t1 != 0)
								{
									printf("\nSIL:%d: Error: NOT does not support arithmetic expressions",root->LINE);
									error++;
								}
								
								return 0;
		
		case True			:
		
		case False			:	return root->TYPE;
		
		case IDADDR			:
		
		case IDFR			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if((lnode == NULL) && (gnode == NULL))
								{
									printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
									error++;
									return 0;
								}
								
								else if((lnode == NULL) && (gnode != NULL) && (gnode->SIZE > 0))
								{
									printf("\nSIL:%d: Error: '%s' has been globally declared as an array",root->LINE,root->NAME);
									error++;
								}
								
								if(lnode != NULL)
									return lnode->TYPE;
								else
									return gnode->TYPE;
		
		case ARRAYIDFR		:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if((lnode == NULL) && (gnode == NULL))
								{
									printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
									error++;
								}
								
								else
								{
									t1 = bodySemanticCheck(root->Ptr1,Lhead);
									
									if(t1 != 0)
									{
										if((t1 != INTGR))
										{
											printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
											error++;
										}
									
										if((lnode != NULL) || (gnode->SIZE == 0))
										{
											printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
											error ++;
										}
										
										if(lnode != NULL)
											return lnode->TYPE;
										else
											return gnode->TYPE;
									}
								}
								
								return 0;
		
		case NUM			:	return INTGR;
		
		default				:	;
	}
}


/*	Check assignment statement ('variable' = 'expression') for type mismatch,
	given that 'variable' is a local variable
	*/
void checkLocalAssign(Tnode *root,struct Lsymbol *lnode,int t1)
{
	char typ1[10],typ2[10];
	
	if(lnode->TYPE == INTGR)
		strcpy(typ1,"integer");
	else if(lnode->TYPE == BOOL)
		strcpy(typ1,"boolean");

	if(t1 == INTGR)
		strcpy(typ2,"integer");
	else if(t1 == BOOL)
		strcpy(typ2,"boolean");

	if((t1 != lnode->TYPE) && (t1 != 0))
	{
		printf("\nSIL:%d: Error: cannot assign '%s' value to '%s' variable",root->LINE,typ2,typ1);
		error++;
	}
}


/*	Check assignment statement ('variable' = 'expression') for type mismatch,
	given that 'variable' is a global variable
	*/
void checkGlobalAssign(Tnode *root,struct Gsymbol *gnode,int t1)
{
	char typ1[10],typ2[10];
	
	if(gnode->TYPE == INTGR)
		strcpy(typ1,"integer");
	else if(gnode->TYPE == BOOL)
		strcpy(typ1,"boolean");

	if(t1 == INTGR)
		strcpy(typ2,"integer");
	else if(t1 == BOOL)
		strcpy(typ2,"boolean");

	if((t1 != gnode->TYPE) && (t1 != 0))
	{
		printf("\nSIL:%d: Error: cannot assign '%s' value to '%s' variable",root->LINE,typ2,typ1);
		error++;
	}
}


/*	Look up an identifier in the global symbol table
	*/
struct Gsymbol *Glookup(char *NAME)
{
	struct Gsymbol *ptr = Ghead;
	
	while(ptr)
	{
		if(!strcmp(ptr->NAME,NAME))
			return ptr;
		
		ptr = ptr->NEXT;
	}
	
	return NULL;
}


/*	Install an identifier in the global symbol table
	*/
void Ginstall(char *NAME,int TYPE,int SIZE,ArgStruct *ARGLIST)
{	
	int i;
	struct Gsymbol *Gnode = malloc(sizeof(struct Gsymbol));
	
	Gnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Gnode->NAME,NAME);
	
	Gnode->ARGLIST = ARGLIST;
	Gnode->TYPE = TYPE;
	Gnode->SIZE = SIZE;
	
	Gnode->LOCATION = getLoc();
	
	if(SIZE > 0)
	{
		for(i=0;i<SIZE-1;i++)
			getLoc();
	}
	
	Gnode->NEXT = Ghead;
	Ghead = Gnode;
}

int getLoc()
{
	return loc++;
}

/*	Look up an identifier in local symbol table (of a function)
	*/
struct Lsymbol *Llookup(char *NAME,struct Lsymbol **Lhead)
{
	struct Lsymbol *ptr = *Lhead;
	
	while(ptr)
	{
		if(!strcmp(ptr->NAME,NAME))
			return ptr;
		
		ptr = ptr->NEXT;
	}
	
	return NULL;
}


/*	Install an identifier in the local symbol table (of a function)
	*/
void Linstall(char *NAME,int TYPE,struct Lsymbol **Lhead)
{
	struct Lsymbol *Lnode = malloc(sizeof(struct Lsymbol));
	
	Lnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Lnode->NAME,NAME);
	
	Lnode->TYPE = TYPE;
	
	Lnode->NEXT = *Lhead;
	*Lhead = Lnode;
}


/*
####################################################
					INTERPRETER
####################################################
*/

int interpreter(Tnode *root,struct Lsymbol **Lhead)
{
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case MAINBLOCK		:	localDecInstall(root->Ptr1,Lhead);
								return evalBody(root->Ptr2,Lhead);
	
		case FUNCBLOCK		:	localDecInstall(root->Ptr2,Lhead);
								return evalBody(root->Ptr3,Lhead);
	}
}

void localDecInstall(Tnode *root,struct Lsymbol **Lhead)
{
	int garbval;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	localDecInstall(root->Ptr1,Lhead);
								localDecInstall(root->Ptr2,Lhead);
								break;
		
		case DECLSTATEMENT	:	decnode = root->Ptr1;
								localDecInstall(root->Ptr2,Lhead);
								break;
		
		case IDFRDECL		:	binding = NULL;
								LinstallBind(root->NAME,decnode->TYPE,garbval,Lhead);
								break;
	}
}


/*	Allocate memory to identifiers in the global symbol table
	*/
void Gallocate()
{
	struct Gsymbol *Gnode = Ghead;
	
	while(Gnode)
	{
		if(Gnode->SIZE > 0)
			Gnode->BINDING = malloc(sizeof(int) * Gnode->SIZE);
		else if(Gnode->SIZE == 0)
			Gnode->BINDING = malloc(sizeof(int));
		
		Gnode = Gnode->NEXT;
	}
}

void LinstallBind(char *NAME,int TYPE,int VALUE,struct Lsymbol **Lhead)
{
	struct Lsymbol *Lnode = malloc(sizeof(struct Lsymbol));
	
	Lnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Lnode->NAME,NAME);
	
	Lnode->TYPE = TYPE;
	
	if(binding == NULL)
	{
		Lnode->BINDING = malloc(sizeof(int));
		*Lnode->BINDING = VALUE;
	}
	
	else Lnode->BINDING = binding;	//For call by reference
	
	Lnode->NEXT = *Lhead;
	*Lhead = Lnode;
}

int evalBody(Tnode *root,struct Lsymbol **Lhead)
{
	int t;
	struct Gsymbol *gnode;
	struct Lsymbol *lnode;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	evalBody(root->Ptr1,Lhead);
								return evalBody(root->Ptr2,Lhead);
		
		case FUNCCALL		:	gnode = Glookup(root->NAME);
								Arghead = gnode->ARGLIST;
								
								struct Lsymbol *Ltable;
								Ltable = NULL;
								
								if(Arghead != NULL)
									funcParamInstall(root->Ptr1,Lhead,&Ltable);
								
								tempnode = searchFunc(root->NAME,funcroot);
								return interpreter(tempnode,&Ltable);
		
		case IDADDR			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									binding = lnode->BINDING;
								else binding = gnode->BINDING;
								
								return *binding;
		
		case RET			:	return evalBody(root->Ptr1,Lhead);
		
		case ITERATIVE		:	while(evalBody(root->Ptr1,Lhead))
									evalBody(root->Ptr2,Lhead);
								return;
		
		case CONDITIONAL	:	if(evalBody(root->Ptr1,Lhead))
									evalBody(root->Ptr2,Lhead);
								else
									evalBody(root->Ptr3,Lhead);
								return;
		
		case ASSIGN			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									*lnode->BINDING = evalBody(root->Ptr1,Lhead);
								else *gnode->BINDING = evalBody(root->Ptr1,Lhead);
								
								return;
		
		case ARRAYASSIGN	:	gnode = Glookup(root->NAME);
								gnode->BINDING[evalBody(root->Ptr1,Lhead)] = evalBody(root->Ptr2,Lhead);
								return;
		
		case RD				:	scanf("%d",&var);
								lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									*lnode->BINDING = var;
								else *gnode->BINDING = var;
								return;
		
		case ARRAYRD		:	scanf("%d",&var);
								gnode = Glookup(root->NAME);
								gnode->BINDING[evalBody(root->Ptr1,Lhead)] = var;
								return;
		
		case WRIT			:	printf("%d\n",evalBody(root->Ptr1,Lhead));
								return;
		
		case ADD			:	return evalBody(root->Ptr1,Lhead) + evalBody(root->Ptr2,Lhead);
		
		case SUB			:	return evalBody(root->Ptr1,Lhead) - evalBody(root->Ptr2,Lhead);
		
		case MUL			:	return evalBody(root->Ptr1,Lhead) * evalBody(root->Ptr2,Lhead);
		
		case DIV			:	return evalBody(root->Ptr1,Lhead) / evalBody(root->Ptr2,Lhead);
		
		case MOD			:	return evalBody(root->Ptr1,Lhead) % evalBody(root->Ptr2,Lhead);
		
		case GT				:	return evalBody(root->Ptr1,Lhead) > evalBody(root->Ptr2,Lhead);
		
		case LT				:	return evalBody(root->Ptr1,Lhead) < evalBody(root->Ptr2,Lhead);
		
		case GTE			:	return evalBody(root->Ptr1,Lhead) >= evalBody(root->Ptr2,Lhead);
		
		case LTE			:	return evalBody(root->Ptr1,Lhead) <= evalBody(root->Ptr2,Lhead);
		
		case EQ				:	return evalBody(root->Ptr1,Lhead) == evalBody(root->Ptr2,Lhead);
		
		case NE				:	return evalBody(root->Ptr1,Lhead) != evalBody(root->Ptr2,Lhead);
		
		case And			:	return evalBody(root->Ptr1,Lhead) && evalBody(root->Ptr2,Lhead);
		
		case Or				:	return evalBody(root->Ptr1,Lhead) || evalBody(root->Ptr2,Lhead);
		
		case Not			:	return !evalBody(root->Ptr1,Lhead);
		
		case True			:	return 1;
		
		case False			:	return 0;
		
		case IDFR			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									return *lnode->BINDING;
								else return *gnode->BINDING;
		
		case ARRAYIDFR		:	gnode = Glookup(root->NAME);
								return gnode->BINDING[evalBody(root->Ptr1,Lhead)];
		
		case NUM			:	return root->VALUE;
		
		default				:	printf("How did flag get this value!");
	}
}

Tnode *searchFunc(char *NAME,Tnode *root)
{
	if(!strcmp(NAME,"main"))
		return mroot;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	tempnode = searchFunc(NAME,root->Ptr1);
								if(tempnode != NULL)
									return tempnode;
								
								else return searchFunc(NAME,root->Ptr2);
		
		case FUNCBLOCK		:	if(!strcmp(NAME,root->NAME))
									return root;
								
								else return NULL;
	}
}

int funcParamInstall(Tnode *root,struct Lsymbol **Lhead,struct Lsymbol **Ltable)
{
	int t;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	funcParamInstall(root->Ptr1,Lhead,Ltable);
								return funcParamInstall(root->Ptr2,Lhead,Ltable);
	
		case FUNCPARAM		:	binding = NULL;
								t = evalBody(root->Ptr1,Lhead);
								LinstallBind(Arghead->NAME,Arghead->TYPE,t,Ltable);
								Arghead = Arghead->NEXT;
								return;
	}
}



/*
####################################################
				CODE GENERATION
####################################################
*/

int codeGenerate(Tnode *root)
{
	int loc,r,r1,r2;
	int lbl1,lbl2;
	struct Gsymbol *TEMP;
	
	if(root==NULL)
		return;

	switch(root->NODETYPE)
	{
		case CONTINUE		:	codeGenerate(root->Ptr1);
								codeGenerate(root->Ptr2);
								
								return;
		
		case ITERATIVE		:	fprintf(fp,"\n*** ITERATION ***\n");
								lbl1 = getLabel();
								lbl2 = getLabel();
								fprintf(fp,"Label%d:\n",lbl1);
								r = codeGenerate(root ->Ptr1);
								fprintf(fp,"JZ R%d Label%d\n",r,lbl2);
								freeReg();
								codeGenerate(root ->Ptr2);
								fprintf(fp,"JMP Label%d\n",lbl1);
								fprintf(fp,"Label%d:\n",lbl2);
								
								return;
		
		case CONDITIONAL	:	fprintf(fp,"\n*** CONDITIONAL ***\n");
								r = codeGenerate(root->Ptr1);
								lbl1 = getLabel();
								lbl2 = getLabel();
								
								fprintf(fp,"JZ R%d Label%d\n",r,lbl1);
								freeReg();
								codeGenerate(root->Ptr2);
								fprintf(fp,"JMP Label%d\n",lbl2);
								fprintf(fp,"Label%d:\n",lbl1);
								codeGenerate(root->Ptr3);
								fprintf(fp,"Label%d:\n",lbl2);
								
								return;
		
		case RD				:	fprintf(fp,"\n*** READ ***\n");
								loc = Glookup(root->NAME)->LOCATION;
								r = getReg();
								fprintf(fp,"IN R%d\n",r);
								fprintf(fp,"MOV [%d] R%d\n",loc,r);
								freeReg();
								
								return -1;
		
		case ARRAYRD		:	fprintf(fp,"\n*** ARRAY READ ***\n");
								r = getReg();
								fprintf(fp,"IN R%d\n",r);
								loc = Glookup(root->NAME)->LOCATION;
								r1 = codeGenerate(root->Ptr1);
								r2 = getReg();
								fprintf(fp,"MOV R%d %d\n",r2,loc);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								fprintf(fp,"MOV [R%d] R%d\n",r1,r);
								freeReg();
								freeReg();
								
								return -1;
		
		case WRIT			:	fprintf(fp,"\n*** WRITE ***\n");
								r = codeGenerate(root->Ptr1);
								fprintf(fp,"OUT R%d\n",r);
								freeReg();
								
								return -1;
		
		case ASSIGN			:	fprintf(fp,"\n*** ASSIGNMENT ***\n");
								loc = Glookup(root->NAME)->LOCATION;
								r = codeGenerate(root->Ptr1);
								fprintf(fp,"MOV [%d] R%d\n",loc,r);
								freeReg();
								
								return;
		
		case GT				:	r = codeGenerate(root->Ptr1);
								codeGenerate(root->Ptr2);
								fprintf(fp,"GT R%d R%d\n",r,r+1);
								freeReg();
								
								return r;
		
		case LT				:	r = codeGenerate(root->Ptr1);
								codeGenerate(root->Ptr2);
								fprintf(fp,"LT R%d R%d\n",r,r+1);
								freeReg();
								
								return r;
		
		case EQ				:	r = codeGenerate(root->Ptr1);
								codeGenerate(root->Ptr2);
								fprintf(fp,"EQ R%d R%d\n",r,r+1);
								freeReg();
								
								return r;
		
		case NE				:	r = codeGenerate(root->Ptr1);
								codeGenerate(root->Ptr2);
								fprintf(fp,"NE R%d R%d\n",r,r+1);
								freeReg();
								
								return r;
		
		case NUM			:	r=getReg();
								fprintf(fp,"MOV R%d %d \n",r,root->VALUE);
								
								return r;
		
		case ADD			:	r=codeGenerate(root->Ptr1);
								r1=codeGenerate(root->Ptr2);
								fprintf(fp,"ADD R%d R%d\n",r,r1);
								freeReg();
								
								return r;
		
		case SUB			:	r=codeGenerate(root->Ptr1);
								r1=codeGenerate(root->Ptr2);
								fprintf(fp,"SUB R%d R%d\n",r,r1);
								freeReg();
								
								return r;
		
		case MUL			:	r=codeGenerate(root->Ptr1);
								r1=codeGenerate(root->Ptr2);
								fprintf(fp,"MUL R%d R%d\n",r,r1);
								freeReg();
								
								return r;
		
		case DIV			:	r=codeGenerate(root->Ptr1);
								r1=codeGenerate(root->Ptr2);
								fprintf(fp,"DIV R%d R%d\n",r,r1);
								freeReg();
								
								return r;
		
		case IDFR			:	loc = Glookup(root->NAME)->LOCATION;
								r=getReg();
								fprintf(fp,"MOV R%d [%d]\n",r,loc);
								
								return r;
		
		case ARRAYIDFR		:	r = codeGenerate(root->Ptr1);
								loc = Glookup(root->NAME)->LOCATION;
								r1 = getReg();
								fprintf(fp,"MOV R%d %d\n",r1,loc);
								fprintf(fp,"ADD R%d R%d\n",r,r1);
								fprintf(fp,"MOV R%d [R%d]\n",r1,r);
								freeReg();
								
								return r1;
	}
}

int getReg()
{
	return regcnt++;
}

void freeReg()
{
	regcnt--;
}

int getLabel()
{
	return ++labelcnt;
}

void freeLabel()
{
	labelcnt--;
}



/*
####################################################
			DISPLAY SYMBOL TABLES
####################################################
*/

void printGlobal()
{
	struct Gsymbol *ptr = Ghead;
	char typ[10];
	char data[10];
	int ctr = 1;

	printf("\nSIL:%d: \n---GLOBAL TABLE---\n");
	while(ptr)
	{
		if(ptr->TYPE == INTGR)
			strcpy(typ,"Integer"); 
		else if(ptr->TYPE == BOOL)
			strcpy(typ,"Boolean");
		
		if(ptr->SIZE>0)
			strcpy(data,"ARRAY");
		else strcpy(data,"VARIABLE");
		
		printf("Entry %d = %s\t%s\t%s\n",ctr,ptr->NAME,typ,data);
		ctr++;
		ptr = ptr->NEXT;
	}
}

void printLocal(struct Lsymbol **HEAD)
{
	struct Lsymbol *ptr = *HEAD;
	char typ[10];
	int ctr = 1;

	printf("\nSIL:%d: \n---GLOBAL TABLE---\n");
	while(ptr)
	{
		if(ptr->TYPE == INTGR)
			strcpy(typ,"Integer"); 
		else if(ptr->TYPE == BOOL)
			strcpy(typ,"Boolean");
		
		printf("Entry %d = %s\t%s\n",ctr,ptr->NAME,typ);
		
		ctr++;
		ptr = ptr->NEXT;
	}
}
