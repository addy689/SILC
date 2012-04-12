/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "semanticChecks.h"


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
								addToFuncList(root);
								
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
	int typ;
	
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
		
		case IDADDRARG		:	typ = returnAddrType(argnode->TYPE);
								
								lnode = Llookup(root->NAME,Lhead);
								if(lnode != NULL)
								{
									if(typ != lnode->TYPE)
										printf("\nSIL:%d: Error: conflicting types for '%s'",root->LINE,root->NAME);
									else
										printf("\nSIL:%d: Error: redefinition of parameter '%s'",root->LINE,root->NAME);
									
									error++;
								}
								
								else Linstall(root->NAME,typ,Lhead);
								
								break;
		
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


/*	Check if function has
	1) A global declaration
	2) Same return type as mentioned in global declaration
	3) No redefinitions
	*/
int funcTypeCheck(Tnode *root)
{
	char *typ;
	
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
			typ = returnTypeString(gtemp->TYPE);
			
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
	int typ;
	ArgStruct *arg;
	
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
		
		case IDADDRARG		:	typ = returnAddrType(argnode->TYPE);
								argIdSemanticCheck(root,typ);
								return;
		
		case IDARG			:	typ = argnode->TYPE;
								argIdSemanticCheck(root,typ);
								return;
	}
}


/*	Check a function argument for semantic errors
	*/
void argIdSemanticCheck(Tnode *root,int TYPE)
{
	char *typ;
	ArgStruct *arg;
	
	arg = argLookup(root->NAME,gtemp->ARGLIST);
	
	if(arg == NULL)
	{
		printf("\nSIL:%d: Error: parameter '%s' not declared in global declaration of function '%s'",root->LINE,root->NAME,gtemp->NAME);
		error++;
	}
	
	else if(arg->FLAG == 0)
	{
		if(TYPE != arg->TYPE)
		{
			typ = returnTypeString(arg->TYPE);
			
			printf("\nSIL:%d: Error: parameter '%s' has type '%s' in global declaration of function '%s'",root->LINE,root->NAME,typ,gtemp->NAME);
			
			error++;
		}
		
		arg->FLAG = 1;
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
	char *typ1,*typ2;
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
								
								typ1 = returnTypeString(functype);
								typ2 = returnTypeString(t1);
								
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
									typ1 = returnTypeString(Ahead->TYPE);
									
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
		
		case IDFR			:	t1 = idSemanticCheck(root,Lhead);
								
								if(t1 == LOCAL)
								{
									lnode = Llookup(root->NAME,Lhead);
									return lnode->TYPE;
								}
								
								else if(t1 == GLOBAL)
								{
									gnode = Glookup(root->NAME);
									return gnode->TYPE;
								}
								
								else return 0;
		
		case ARRAYIDFR		:	t1 = arrayIdSemanticCheck(root,Lhead);
								
								if(t1 == LOCAL)
								{
									lnode = Llookup(root->NAME,Lhead);
									return lnode->TYPE;
								}
								
								else if(t1 == GLOBAL)
								{
									gnode = Glookup(root->NAME);
									return gnode->TYPE;
								}
								
								else return 0;
		
		case IDADDR			:	t1 = idSemanticCheck(root,Lhead);
								
								if(t1 == LOCAL)
								{
									lnode = Llookup(root->NAME,Lhead);
									return returnAddrType(lnode->TYPE);
								}
								
								else if(t1 == GLOBAL)
								{
									gnode = Glookup(root->NAME);
									return returnAddrType(gnode->TYPE);
								}
								
								else return 0;
		
		case ARRAYIDADDR	:	t1 = arrayIdSemanticCheck(root,Lhead);
								
								if(t1 == LOCAL)
								{
									lnode = Llookup(root->NAME,Lhead);
									return returnAddrType(lnode->TYPE);
								}
								
								else if(t1 == GLOBAL)
								{
									gnode = Glookup(root->NAME);
									return returnAddrType(gnode->TYPE);
								}
								
								else return 0;
		
		case NUM			:	return INTGR;
		
		default				:	;
	}
}


/*	Check for local/global declaration of an identifier
	*/
int idSemanticCheck(Tnode *root,struct Lsymbol **Lhead)
{
	struct Lsymbol *lnode;
	struct Gsymbol *gnode;
	
	lnode = Llookup(root->NAME,Lhead);
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
		return LOCAL;
	else
		return GLOBAL;
}


/*	Check for local/global declaration of an array identifier
	*/
int arrayIdSemanticCheck(Tnode *root,struct Lsymbol **Lhead)
{
	int t1;
	struct Lsymbol *lnode;
	struct Gsymbol *gnode;
	
	lnode = Llookup(root->NAME,Lhead);
	gnode = Glookup(root->NAME);
	
	if((lnode == NULL) && (gnode == NULL))
	{
		printf("\nSIL:%d: Error: '%s' is an undeclared identifier",root->LINE,root->NAME);
		error++;
	}
	
	else if((lnode != NULL) || (gnode->SIZE == 0))
	{
		printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
		error ++;
	}
	
	t1 = bodySemanticCheck(root->Ptr1,Lhead);
	
	if((t1 != 0) && (t1 != INTGR))
	{
		printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
		error++;
	}
	
	if(lnode != NULL)
		return LOCAL;
	else if(gnode!= NULL)
		return GLOBAL;
	else return 0;
}


/*	Check assignment statement ('variable' = 'expression') for type mismatch,
	given that 'variable' is a local variable
	*/
void checkLocalAssign(Tnode *root,struct Lsymbol *lnode,int exprtype)
{
	char *typ1,*typ2;
	int t1,t2;
	int cond1,cond2;
	
	cond1 = (lnode->TYPE == INTGR || lnode->TYPE == INTGRALIAS);
	cond2 = (exprtype == INTGR) || (exprtype == INTGRALIAS);
	
	if(cond1)
		t1 = INTGR;
	else t1 = BOOL;
	
	if(cond2)
		t2 = INTGR;
	else t2 = BOOL;
	
	
	if((t1 != t2) && (exprtype != 0))
	{
		typ1 = returnTypeString(t1);
		typ2 = returnTypeString(t2);
		
		printf("\nSIL:%d: Error: cannot assign '%s' value to '%s' variable",root->LINE,typ2,typ1);
		error++;
	}
}


/*	Check assignment statement ('variable' = 'expression') for type mismatch,
	given that 'variable' is a global variable
	*/
void checkGlobalAssign(Tnode *root,struct Gsymbol *gnode,int exprtype)
{
	char *typ1,*typ2;
	int t1,t2;
	int cond1,cond2;
	
	cond1 = (gnode->TYPE == INTGR || gnode->TYPE == INTGRALIAS);
	cond2 = (exprtype == INTGR) || (exprtype == INTGRALIAS);
	
	if(cond1)
		t1 = INTGR;
	else t1 = BOOL;
	
	if(cond2)
		t2 = INTGR;
	else t2 = BOOL;
	
	
	if((t1 != t2) && (exprtype != 0))
	{
		typ1 = returnTypeString(t1);
		typ2 = returnTypeString(t2);
		
		printf("\nSIL:%d: Error: cannot assign '%s' value to '%s' variable",root->LINE,typ2,typ1);
		error++;
	}
}


/* Returns appropriate type as string
	*/
char *returnTypeString(int TYPE)
{
	if(TYPE == INTGR)
		return "integer";
	
	else if(TYPE == BOOL)
		return "boolean";
	
	else if(TYPE == INTGRALIAS)
		return "integer address";
	
	else if(TYPE == BOOLALIAS)
		return "boolean address";
}
