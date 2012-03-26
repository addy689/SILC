#include "compilerLib.h"

Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3,int LINE)
{
	Tnode *N = malloc(sizeof(Tnode));
	
	N->TYPE = TYPE;
	N->NODETYPE = NODETYPE;
	
	N->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(N->NAME,NAME);
	N->VALUE = VALUE;
	
	N->Ptr1 = Ptr1;
	N->Ptr2 = Ptr2;
	N->Ptr3 = Ptr3;
	
	N->LINE = LINE;
	
	return N;
}

void compile(Tnode *declroot, Tnode *stroot)
{
	error = 0;
	globalInstall(declroot);
	semanticCheck(stroot);
	printf("\n%d",line);
	if(error==0)
	{
		gAllocate();
		ex(stroot);
	}
}

void globalInstall(Tnode *root)
{
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	globalInstall(root->Ptr1);
								globalInstall(root->Ptr2);
								break;
		
		case DECLSTATEMENT	:	tempnode = root->Ptr1;
								globalInstall(root->Ptr2);
								break;
		
		case IDFRDECL		:
		
		case ARRAYDECL		:	if(gLookup(root->NAME))
								{
									printf("Variable %s already declared\n",root->NAME);
									error++;
								}
								else
									gInstall(root->NAME,tempnode->TYPE,root->VALUE);
								break;
	}
}

struct Gsymbol *gLookup(char *NAME)
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

void gInstall(char *NAME,int TYPE,int SIZE)
{
	struct Gsymbol *Gnode = malloc(sizeof(struct Gsymbol));
	
	Gnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Gnode->NAME,NAME);
	
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

struct Gsymbol *checkIdentDecl(char *NAME,int LINE)
{
	struct Gsymbol *ptr;
	
	ptr = gLookup(NAME);
	if(ptr == NULL)
	{
		printf("\nSIL:%d: Error: %s is an undeclared identifier",LINE,NAME);
		return NULL;
	}
	
	return ptr;
}

int semanticCheck(Tnode *root)
{
	if(root == NULL)
		return 0;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	semanticCheck(root->Ptr1);
								semanticCheck(root->Ptr2);
								return;
		
		case ITERATIVE		:	{	int t1;
									t1 = semanticCheck(root->Ptr1);
									
									if(t1!=BOOL && t1!=0)
									{
										printf("\nSIL:%d: Error: while condition is not a logical expression",root->LINE);
										error++;
									}
								}
								
								semanticCheck(root->Ptr2);
								return;

		case CONDITIONAL	:	{	int t1;
									t1 = semanticCheck(root->Ptr1);
									
									if(t1!=BOOL && t1!=0)
									{
										printf("\nSIL:%d: Error: if condition is not a logical expression",root->LINE);
										error++;
									}
								}
								
								semanticCheck(root->Ptr2);
								semanticCheck(root->Ptr3);
								return;
		
		case ASSIGN			:	{	int flag,t1;
									char typ1[10],typ2[10];
									struct Gsymbol *gnode;
									
									gnode = checkIdentDecl(root->NAME,root->LINE);
									if(!gnode)
									{
										flag = 0;
										error++;
									}
								
									t1 = semanticCheck(root->Ptr1);
									
									if(flag!=0)
									{
										if(gnode->TYPE==INTGR)
											strcpy(typ1,"integer");
										else if(gnode->TYPE==BOOL)
											strcpy(typ1,"boolean");
									
										if(t1==INTGR)
											strcpy(typ2,"integer");
										else if(t1==BOOL)
											strcpy(typ2,"boolean");
									
										if(t1!=gnode->TYPE && t1!=0)
										{
											printf("\nSIL:%d: Error: cannot assign %s value to %s variable",root->LINE,typ1,typ2);
											error++;
										}
									
										else if(gnode->SIZE>0)
										{
											printf("\nSIL:%d: Error: incompatible types when assigning to type %s[%d] from type %s variable",root->LINE,typ1,gnode->SIZE,typ2);
											error++;
										}
									}
								}
								
								return 0;
		
		case ARRAYASSIGN	:	{	int flag,t1;
									char typ1[10],typ2[10];
									struct Gsymbol *gnode;
									
									gnode = checkIdentDecl(root->NAME,root->LINE);
									if(!gnode)
									{
										flag = 0;
										error++;
									}
									
									t1 = semanticCheck(root->Ptr1);
									
									if(t1!=INTGR && t1!=0)
									{
										printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
										error++;
									}
								
									else if(gnode->SIZE==0 && t1!=0)
									{
										printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
										error ++;
									}
									
									t1 = semanticCheck(root->Ptr2);
									
									if(flag!=0)
									{
										if(gnode->TYPE==INTGR)
											strcpy(typ1,"integer");
										else if(gnode->TYPE==BOOL)
											strcpy(typ1,"boolean");
							
										if(t1==INTGR)
											strcpy(typ2,"integer");
										else if(t1==BOOL)
											strcpy(typ2,"boolean");
						
										if(t1!=gnode->TYPE && t1!=0)
										{
											printf("\nSIL:%d: Error: cannot assign %s value to %s variable",root->LINE,typ1,typ2);
											error++;
										}
									}
								}
								
								return 0;
		
		case RD				:	gtemp = checkIdentDecl(root->NAME,root->LINE);
								
								if(!gtemp)
									error++;
								
								else if(gtemp->TYPE == BOOL)
								{
									printf("\nSIL:%d: Error: read operation is not allowed on boolean variables",root->LINE);
									error++;
								}
								
								return 0;
		
		case ARRAYRD		:	gtemp = checkIdentDecl(root->NAME,root->LINE);
								
								if(!gtemp)
									error++;
								
								else if(gtemp->TYPE == BOOL)
								{
									printf("\nSIL:%d: Error: read operation is not allowed on boolean variables",root->LINE);
									error++;
								}
								
								else
								{
									int t1;
									t1 = semanticCheck(root->Ptr1);
									gtemp = gLookup(root->NAME);
									
									if(t1!=INTGR && t1!=0)
									{
										printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
										error++;
									}
									
									else if(gtemp->SIZE==0 && t1!=0)
									{
										printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
										error ++;
									}
								}
								
								return 0;
		
		case WRIT			:	{	int t1;
									t1 = semanticCheck(root->Ptr1);
									if(t1!=INTGR && t1!=0)
									{
										printf("\nSIL:%d: Error: write operation not allowed on boolean variables",root->LINE);
										error++;
									}
								}
								return 0;
		
		case DIV			:	if(root->Ptr2->VALUE==0 && root->Ptr2->NODETYPE==NUM)
									printf("\nSIL:%d: Warning: division by zero",root->LINE);
								
								{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
								
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: / does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case ADD			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: + does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case SUB			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);

									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: - does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case MUL			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: * does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case MOD			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: %% does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case GT				:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: > does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case LT				:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: < does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case GTE			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: >= does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case LTE			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: <= does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case EQ				:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: == does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case NE				:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==INTGR && t2==INTGR)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: != does not support logical expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case And			:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==BOOL && t2==BOOL)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: AND does not support arithmetic expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case Or				:	{	int t1,t2;
									t1 = semanticCheck(root->Ptr1);
									t2 = semanticCheck(root->Ptr2);
									
									if(t1==BOOL && t2==BOOL)
										return root->TYPE;
									
									else if(!(t1==0 || t2==0))
									{
										printf("\nSIL:%d: Error: OR does not support arithmetic expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case Not			:	{	int t1;
									t1 = semanticCheck(root->Ptr1);
									
									if(t1==BOOL)
										return root->TYPE;
									
									else if(t1!=0)
									{
										printf("\nSIL:%d: Error: NOT does not support arithmetic expressions",root->LINE);
										error++;
									}
								}
								return 0;
		
		case True			:
		
		case False			:	return root->TYPE;
		
		case IDFR			:	gtemp = checkIdentDecl(root->NAME,root->LINE);
								
								if(!gtemp)
								{
									error++;
									return 0;
								}
								
								else if(gtemp->SIZE>0)
									error++;
								
								return gtemp->TYPE;
		
		case ARRAYIDFR		:	gtemp = checkIdentDecl(root->NAME,root->LINE);
								
								if(!gtemp)
									error++;
								
								else
								{
									int t1;
									t1 = semanticCheck(root->Ptr1);
									gtemp = gLookup(root->NAME);
									
									if(t1!=INTGR && t1!=0)
									{
										printf("\nSIL:%d: Error: array subscript is not an integer",root->LINE);
										error++;
									}
									
									else if(gtemp->SIZE==0 && t1!=0)
									{
										printf("\nSIL:%d: Error: subscripted value is not an array",root->LINE);
										error ++;
									}
									
									else return gtemp->TYPE;
								}
								
								return 0;
		
		case NUM			:	return INTGR;
		
		default				:	;
	}
}

int ex(Tnode *root)
{
	if(root==NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	ex(root->Ptr1);
								ex(root->Ptr2);
								break;
		
		case ITERATIVE		:	while(ex(root->Ptr1))
									ex(root->Ptr2);
								return ex(root->Ptr3);
		
		case CONDITIONAL	:	if(ex(root->Ptr1))
									ex(root->Ptr2);
								else
									ex(root->Ptr3);
								return;
		
		case ASSIGN			:	gtemp = gLookup(root->NAME);
								*gtemp->BINDING = ex(root->Ptr1);
								return;
		
		case ARRAYASSIGN	:	gtemp = gLookup(root->NAME);
								gtemp->BINDING[ex(root->Ptr1)] = ex(root->Ptr2);
								return;
		
		case RD				:	scanf("%d",&var);
								gtemp = gLookup(root->NAME);
								*gtemp->BINDING = var;
								return;
		
		case ARRAYRD		:	scanf("%d",&var);
								gtemp = gLookup(root->NAME);
								gtemp->BINDING[ex(root->Ptr1)] = var;
								return;
		
		case WRIT			:	printf("%d\n",ex(root->Ptr1));
								return;
		
		case ADD			:	return ex(root->Ptr1) + ex(root->Ptr2);
		
		case SUB			:	return ex(root->Ptr1) - ex(root->Ptr2);
		
		case MUL			:	return ex(root->Ptr1) * ex(root->Ptr2);
		
		case DIV			:	return ex(root->Ptr1) / ex(root->Ptr2);
		
		case MOD			:	return ex(root->Ptr1) % ex(root->Ptr2);
		
		case GT				:	return ex(root->Ptr1) > ex(root->Ptr2);
		
		case LT				:	return ex(root->Ptr1) < ex(root->Ptr2);
		
		case GTE			:	return ex(root->Ptr1) >= ex(root->Ptr2);
		
		case LTE			:	return ex(root->Ptr1) <= ex(root->Ptr2);
		
		case EQ				:	return ex(root->Ptr1) == ex(root->Ptr2);
		
		case NE				:	return ex(root->Ptr1) != ex(root->Ptr2);
		
		case And			:	return ex(root->Ptr1) && ex(root->Ptr2);
		
		case Or				:	return ex(root->Ptr1) || ex(root->Ptr2);
		
		case Not			:	return !ex(root->Ptr1);
		
		case True			:	return 1;
		
		case False			:	return 0;
		
		case IDFR			:	gtemp = gLookup(root->NAME);
								return *gtemp->BINDING;
		
		case ARRAYIDFR		:	gtemp = gLookup(root->NAME);
								return gtemp->BINDING[ex(root->Ptr1)];
		
		case NUM			:	return root->VALUE;
		
		default				:	printf("How did flag get this value!");
	}
}

/*void printGlobal()*/
/*{*/
/*	struct Gsymbol *ptr = Ghead;*/
/*	char typ[10];*/
/*	char data[10];*/
/*	int ctr = 1;*/

/*	printf("\nSIL:%d: \n---GLOBAL TABLE---\n");*/
/*	while(ptr)*/
/*	{*/
/*		if(ptr->TYPE == INTGR)*/
/*			strcpy(typ,"Integer"); */
/*		else if(ptr->TYPE == BOOL)*/
/*			strcpy(typ,"Boolean");*/
/*		*/
/*		if(ptr->SIZE>0)*/
/*			strcpy(data,"ARRAY");*/
/*		else strcpy(data,"VARIABLE");*/
/*		*/
/*		printf("Entry %d = %s\t%s\t%s\n",ctr,ptr->NAME,typ,data);*/
/*		ctr++;*/
/*		ptr = ptr->NEXT;*/
/*	}*/
/*}*/
