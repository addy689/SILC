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
	int t,*loc;
	struct Gsymbol *gnode;
	struct Lsymbol *lnode;
	ArgStruct *Ahead;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	evalBody(root->Ptr1,Lhead);
								return evalBody(root->Ptr2,Lhead);
		
		case FUNCCALL		:	gnode = Glookup(root->NAME);
								Ahead = gnode->ARGLIST;
								
								struct Lsymbol *Ltable;
								Ltable = NULL;
								
								if(Ahead != NULL)
									funcParamInstall(root->Ptr1,Lhead,&Ltable,&Ahead);
								
								tempnode = funcLookup(root->NAME);
								return interpreter(tempnode,&Ltable);
		
		case IDADDR			:	binding = HlookupInterpreter(root->NAME,Lhead);
								return;
		
		case ARRAYIDADDR	:	gnode = Glookup(root->NAME);
								t = evalBody(root->Ptr1,Lhead);
								binding = gnode->LOCATION + t;
								return;
		
		case RET			:	return evalBody(root->Ptr1,Lhead);
		
		case ITERATIVE		:	while(evalBody(root->Ptr1,Lhead))
									evalBody(root->Ptr2,Lhead);
								return;
		
		case CONDITIONAL	:	if(evalBody(root->Ptr1,Lhead))
									evalBody(root->Ptr2,Lhead);
								else
									evalBody(root->Ptr3,Lhead);
								return;
		
		case ASSIGN			:	loc = HlookupInterpreter(root->NAME,Lhead);
								*loc = evalBody(root->Ptr1,Lhead);
								return;
		
		case ARRAYASSIGN	:	gnode = Glookup(root->NAME);
								gnode->LOCATION[evalBody(root->Ptr1,Lhead)] = evalBody(root->Ptr2,Lhead);
								return;
		
		case RD				:	scanf("%d",&var);
								loc = HlookupInterpreter(root->NAME,Lhead);
								*loc = var;
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
		
		case IDFR			:	loc = HlookupInterpreter(root->NAME,Lhead);
								return *loc;
		
		case ARRAYIDFR		:	gnode = Glookup(root->NAME);
								return gnode->LOCATION[evalBody(root->Ptr1,Lhead)];
		
		case NUM			:	return root->VALUE;
		
		default				:	printf("How did flag get this value!");
	}
}

int *HlookupInterpreter(char *NAME,struct Lsymbol **Lhead)
{
	struct Lsymbol *lnode;
	struct Gsymbol *gnode;
	int type;
	
	lnode = Llookup(NAME,Lhead);
	gnode = Glookup(NAME);
	
	if(lnode != NULL)
		return lnode->LOCATION;
	else return gnode->LOCATION;
	
}

int funcParamInstall(Tnode *root,struct Lsymbol **Lhead,struct Lsymbol **Ltable,ArgStruct **Ahead)
{
	int t;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	funcParamInstall(root->Ptr1,Lhead,Ltable,Ahead);
								return funcParamInstall(root->Ptr2,Lhead,Ltable,Ahead);
	
		case FUNCPARAM		:	binding = NULL;
								t = evalBody(root->Ptr1,Lhead);
								LinstallBind((*Ahead)->NAME,(*Ahead)->TYPE,t,Ltable);
								*Ahead = (*Ahead)->NEXT;
								return;
	}
}
