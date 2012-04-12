/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "interpret.h"


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
								
								tempnode = funcLookup(root->NAME);
								return interpreter(tempnode,&Ltable);
		
		case IDADDR			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									binding = lnode->LOCATION;
								else binding = gnode->LOCATION;
								
								return *binding;
		
		case ARRAYIDADDR	:	gnode = Glookup(root->NAME);
								t = evalBody(root->Ptr1,Lhead);
								binding = gnode->LOCATION + t;
								
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
									*lnode->LOCATION = evalBody(root->Ptr1,Lhead);
								else *gnode->LOCATION = evalBody(root->Ptr1,Lhead);
								
								return;
		
		case ARRAYASSIGN	:	gnode = Glookup(root->NAME);
								gnode->LOCATION[evalBody(root->Ptr1,Lhead)] = evalBody(root->Ptr2,Lhead);
								return;
		
		case RD				:	scanf("%d",&var);
								lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									*lnode->LOCATION = var;
								else *gnode->LOCATION = var;
								return;
		
		case ARRAYRD		:	scanf("%d",&var);
								gnode = Glookup(root->NAME);
								gnode->LOCATION[evalBody(root->Ptr1,Lhead)] = var;
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
									return *lnode->LOCATION;
								else return *gnode->LOCATION;
		
		case ARRAYIDFR		:	gnode = Glookup(root->NAME);
								return gnode->LOCATION[evalBody(root->Ptr1,Lhead)];
		
		case NUM			:	return root->VALUE;
		
		default				:	printf("How did flag get this value!");
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
