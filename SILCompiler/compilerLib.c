#include "compilerLib.h"

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

void compile(Tnode *gdeclroot,Tnode *fdefroot,Tnode *mainroot)
{
	error = 0;
	globalInstall(gdeclroot);
	funcSemanticCheck(fdefroot);
	checkFuncDecl(fdefroot->LINE);
	funcSemanticCheck(mainroot);
	
	printf("\n");
	if(error == 0)
	{
		gAllocate();
		//ex(fdefroot);
	}
}

void printGlobal(struct Lsymbol **HEAD)
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
		
		case FUNCBLOCK		:	//installs function arguments in local table (with semantic checking)
								argLocalInstall(root->ArgList,&Lhead);
								
//checks if the function has a global declaration, and checks the function return type. Also checks if the function is a redeclaration
								gtemp = Glookup(root->NAME);
								functype = root->Ptr1->TYPE;
								
								funcTypeCheck(root);
								
								if(gtemp)
								{
									//checks the names and types of arguments with the global declaration of function arguments
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

//installs a function's arguments into that function's Local Symbol table
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
		
//checks argument against function global arguments stored in linked list having head = gtemp->ARGLIST
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

void helplocal(Tnode *root,struct Lsymbol *lnode,int t1)
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

void helpglobal(Tnode *root,struct Gsymbol *gnode,int t1)
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
									helplocal(root,lnode,t1);
								
								else if(gnode != NULL)
								{
									helpglobal(root,gnode,t1);
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
									helplocal(root,lnode,t1);
								
								else if(gnode != NULL)
									helpglobal(root,gnode,t1);
								
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
								
								else if((lnode->TYPE == BOOL) || (gnode->TYPE == BOOL))
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

void Ginstall(char *NAME,int TYPE,int SIZE,ArgStruct *ARGLIST)
{
	struct Gsymbol *Gnode = malloc(sizeof(struct Gsymbol));
	
	Gnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Gnode->NAME,NAME);

	Gnode->ARGLIST = ARGLIST;
	Gnode->TYPE = TYPE;
	Gnode->SIZE = SIZE;
	
	Gnode->NEXT = Ghead;
	Ghead = Gnode;
}

void gAllocate()
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

void Linstall(char *NAME,int TYPE,struct Lsymbol **Lhead)
{
	struct Lsymbol *Lnode = malloc(sizeof(struct Lsymbol));
	
	Lnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Lnode->NAME,NAME);
	
	Lnode->TYPE = TYPE;
	
	Lnode->NEXT = *Lhead;
	*Lhead = Lnode;
}

int ex(Tnode *root,struct Lsymbol **Lhead)
{
	struct Gsymbol *gnode;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	ex(root->Ptr1,Lhead);
								ex(root->Ptr2,Lhead);
								break;
		
		case ITERATIVE		:	while(ex(root->Ptr1,Lhead))
									ex(root->Ptr2,Lhead);
								return;
		
		case CONDITIONAL	:	if(ex(root->Ptr1,Lhead))
									ex(root->Ptr2,Lhead);
								else
									ex(root->Ptr3,Lhead);
								return;
		
		case ASSIGN			:	gnode = Glookup(root->NAME);
								*gnode->BINDING = ex(root->Ptr1,Lhead);
								return;
		
		case ARRAYASSIGN	:	gnode = Glookup(root->NAME);
								gnode->BINDING[ex(root->Ptr1,Lhead)] = ex(root->Ptr2,Lhead);
								return;
		
		case RD				:	scanf("%d",&var);
								gnode = Glookup(root->NAME);
								*gnode->BINDING = var;
								return;
		
		case ARRAYRD		:	scanf("%d",&var);
								gnode = Glookup(root->NAME);
								gnode->BINDING[ex(root->Ptr1,Lhead)] = var;
								return;
		
		case WRIT			:	printf("%d\n",ex(root->Ptr1,Lhead));
								return;
		
		case ADD			:	return ex(root->Ptr1,Lhead) + ex(root->Ptr2,Lhead);
		
		case SUB			:	return ex(root->Ptr1,Lhead) - ex(root->Ptr2,Lhead);
		
		case MUL			:	return ex(root->Ptr1,Lhead) * ex(root->Ptr2,Lhead);
		
		case DIV			:	return ex(root->Ptr1,Lhead) / ex(root->Ptr2,Lhead);
		
		case MOD			:	return ex(root->Ptr1,Lhead) % ex(root->Ptr2,Lhead);
		
		case GT				:	return ex(root->Ptr1,Lhead) > ex(root->Ptr2,Lhead);
		
		case LT				:	return ex(root->Ptr1,Lhead) < ex(root->Ptr2,Lhead);
		
		case GTE			:	return ex(root->Ptr1,Lhead) >= ex(root->Ptr2,Lhead);
		
		case LTE			:	return ex(root->Ptr1,Lhead) <= ex(root->Ptr2,Lhead);
		
		case EQ				:	return ex(root->Ptr1,Lhead) == ex(root->Ptr2,Lhead);
		
		case NE				:	return ex(root->Ptr1,Lhead) != ex(root->Ptr2,Lhead);
		
		case And			:	return ex(root->Ptr1,Lhead) && ex(root->Ptr2,Lhead);
		
		case Or				:	return ex(root->Ptr1,Lhead) || ex(root->Ptr2,Lhead);
		
		case Not			:	return !ex(root->Ptr1,Lhead);
		
		case True			:	return 1;
		
		case False			:	return 0;
		
		case IDFR			:	gnode = Glookup(root->NAME);
								return *gnode->BINDING;
		
		case ARRAYIDFR		:	gnode = Glookup(root->NAME);
								return gnode->BINDING[ex(root->Ptr1,Lhead)];
		
		case NUM			:	return root->VALUE;
		
		default				:	printf("How did flag get this value!");
	}
}
