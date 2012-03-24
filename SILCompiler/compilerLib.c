#include "compilerLib.h"

Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3)
{
	printf("\nAllocating\n");
	Tnode *N = malloc(sizeof(Tnode));
	
	N->TYPE = TYPE;
	N->NODETYPE = NODETYPE;
	
	N->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(N->NAME,NAME);
	N->VALUE = VALUE;
	
	N->Ptr1 = Ptr1;
	N->Ptr2 = Ptr2;
	N-> Ptr3 = Ptr3;
	
	return N;
}

void compile(Tnode *declroot, Tnode *stroot)
{
	error = 0;
	globalInstall(declroot);
	semanticCheck(stroot);
	
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
									printf("Variable %s already declared\n",root->NAME);
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

struct Gsymbol *checkIdentDecl(char *NAME)
{
	struct Gsymbol *ptr;
	
	ptr = gLookup(NAME);
	if(ptr == NULL)
	{
		printf("%s is an undeclared identifier\n",NAME);
		return NULL;
	}
	
	return ptr;
}

void semanticCheck(Tnode *root)
{
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	semanticCheck(root->Ptr1);
								semanticCheck(root->Ptr2);
								break;
		
		case ASSIGN			:	if(!checkIdentDecl(root->NAME))
									error++;
								semanticCheck(root->Ptr1);
								break;
		
		case ARRAYASSIGN	:	if(!checkIdentDecl(root->NAME))
									error++;
								semanticCheck(root->Ptr1);
								semanticCheck(root->Ptr2);
								break;
		
		case RD				:
		
		case ARRAYRD		:	gtemp = checkIdentDecl(root->NAME);
								if(!gtemp)
									error++;
								if(gtemp->TYPE == BOOL)
								{
									printf("Read operation not allowed on boolean variables\n");
									error++;
								}
								semanticCheck(root->Ptr1);
								break;
		
		case WRIT			:	semanticCheck(root->Ptr1);
								break;
		
		case DIV			:	if(root->Ptr2->VALUE == 0)
									printf("Warning: division by zero\n");
		
		case ADD			:
		
		case SUB			:
		
		case MUL			:	semanticCheck(root->Ptr1);
								semanticCheck(root->Ptr2);
								break;
		
		case IDFR			:	if(!checkIdentDecl(root->NAME))
									error++;
								semanticCheck(root->Ptr1);
								break;
		
		case ARRAYIDFR		:	if(!checkIdentDecl(root->NAME))
									error++;
								semanticCheck(root->Ptr1);
								break;
		
		case NUM			:	semanticCheck(root->Ptr1);
								break;
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
		
		case GT				:	return ex(root->Ptr1) > ex(root->Ptr2);
		
		case LT				:	return ex(root->Ptr1) < ex(root->Ptr2);
		
		case GTE			:	return ex(root->Ptr1) >= ex(root->Ptr2);
		
		case LTE			:	return ex(root->Ptr1) <= ex(root->Ptr2);
		
		case EQ				:	return ex(root->Ptr1) == ex(root->Ptr2);
		
		case NE				:	return ex(root->Ptr1) != ex(root->Ptr2);
		
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

/*	printf("\n\n---GLOBAL TABLE---\n");*/
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
