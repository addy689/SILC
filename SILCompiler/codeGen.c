/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "codeGen.h"


void callToMain()
{
	fprintf(fp,"START\n");
	fprintf(fp,"MOV SP -1\n");
	pushGlobalVar();
	
	fprintf(fp,"CALL MAIN\n");
	fprintf(fp,"HALT\n");
}

void funcCodeGen(Tnode *root)
{
	struct Gsymbol *gnode;
	struct Lsymbol *Lhead;
	
	Lhead = NULL;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	funcCodeGen(root->Ptr1);
								funcCodeGen(root->Ptr2);
								return;
		
		case FUNCBLOCK		:	gnode = Glookup(root->NAME);
								
								locneg = -2;
								argInstallLocal(gnode->ARGLIST,&Lhead);
								
								locpos = 0;
								idstatus = 0;
								localDecInstall(root->Ptr2,&Lhead);
								
								fprintf(fp,"\nLABEL%d:\n",gnode->BINDING);
								fprintf(fp,"PUSH BP\n");
								fprintf(fp,"MOV BP SP\n");
								
								pushLocalVar(&Lhead);
								codeGenerate(root->Ptr3,&Lhead);
								
								return;
		
		case MAINBLOCK		:	fprintf(fp,"MAIN:\n");
								fprintf(fp,"PUSH BP\n");
								fprintf(fp,"MOV BP SP\n");
								
								locpos = 0;
								idstatus = 0;
								localDecInstall(root->Ptr1,&Lhead);
								
								pushLocalVar(&Lhead);
								
								codeGenerate(root->Ptr2,&Lhead);
								
								return;
	}
}


/*	Installs function arguments into that function's local symbol table (using the argument list stored in global symbol table entry of function)
	*/
void argInstallLocal(ArgStruct *HEAD,struct Lsymbol **Lhead)
{
	ArgStruct *ptr = HEAD;
	int garbval;
	
	idstatus = ARGUMENT;
	while(ptr)
	{
		LinstallBind(ptr->NAME,ptr->TYPE,garbval,Lhead);
		ptr = ptr->NEXT;
	}
}


int codeGenerate(Tnode *root,struct Lsymbol **Lhead)
{
	int r0,r1,rc;
	int lbl1,lbl2;
	struct Gsymbol *gnode;
	
	if(root==NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	codeGenerate(root->Ptr1,Lhead);
								codeGenerate(root->Ptr2,Lhead);
								return;
		
		case FUNCCALL		:	rc = regcnt;
								pushAllRegs();
								pushCallParams(root->Ptr1,Lhead);
								
								r0 = getReg();
								fprintf(fp,"PUSH R%d\n",r0);
								freeReg();
								
								gnode = Glookup(root->NAME);
								fprintf(fp,"CALL LABEL%d\n",gnode->BINDING);
								
								regcnt = rc;
								r0 = getReg();
								fprintf(fp,"POP R%d\n",r0);
								
								popAllArgs(gnode->ARGLIST);
								
								restoreRegs(rc);
								
								return r0;
		
		case IDADDR			:	idtype = IDADDR;
								return Hlookup(root->NAME,Lhead);
		
		case ARRAYIDADDR	:	idtype = IDADDR;
								r0 = Hlookup(root->NAME,Lhead);
								r1 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case RET			:	r0 = getReg();
								fprintf(fp,"MOV R%d BP\n",r0);
								
								r1 = getReg();
								fprintf(fp,"MOV R%d 2\n",r1);
								fprintf(fp,"SUB R%d R%d\n",r0,r1);
								freeReg();
								
								r1 = codeGenerate(root->Ptr1,Lhead);
								
								fprintf(fp,"MOV [R%d] R%d\n",r0,r1);
								freeReg();
								freeReg();
								
								popAllLocal(Lhead);
								fprintf(fp,"POP BP\n");
								fprintf(fp,"RET\n");
								return;
		
		case ITERATIVE		:	lbl1 = getLabel();
								lbl2 = getLabel();
								
								fprintf(fp,"LABEL%d:\n",lbl1);
								
								r0 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"JZ R%d LABEL%d\n",r0,lbl2);
								freeReg();
								
								codeGenerate(root->Ptr2,Lhead);
								
								fprintf(fp,"JMP LABEL%d\n",lbl1);
								fprintf(fp,"LABEL%d:\n",lbl2);
								return;
		
		case CONDITIONAL	:	lbl1 = getLabel();
								lbl2 = getLabel();
								
								r0 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"JZ R%d LABEL%d\n",r0,lbl1);
								freeReg();
								
								codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"JMP LABEL%d\n",lbl2);
								
								fprintf(fp,"LABEL%d:\n",lbl1);
								codeGenerate(root->Ptr3,Lhead);
								
								fprintf(fp,"LABEL%d:\n",lbl2);
								return;
		
		case RD				:	idtype = 0;
								r0 = Hlookup(root->NAME,Lhead);
								
								r1 = getReg();
								fprintf(fp,"IN R%d\n",r1);
								
								fprintf(fp,"MOV [R%d] R%d\n",r0,r1);
								freeReg();
								freeReg();
								return;
		
		case ARRAYRD		:	idtype = 0;
								r0 = Hlookup(root->NAME,Lhead);
								r1 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r0,r1);
								freeReg();
								
								r1 = getReg();
								fprintf(fp,"IN R%d\n",r1);
								
								fprintf(fp,"MOV [R%d] R%d\n",r0,r1);
								freeReg();
								freeReg();
								return;
		
		case WRIT			:	r0 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"OUT R%d\n",r0);
								freeReg();
								return;
		
		case ASSIGN			:	idtype = 0;
								r0 = Hlookup(root->NAME,Lhead);
								r1 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"MOV [R%d] R%d\n",r0,r1);
								freeReg();
								freeReg();
								return;
		
		case ARRAYASSIGN	:	idtype = 0;
								r0 = Hlookup(root->NAME,Lhead);
								r1 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r0,r1);
								freeReg();
								
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MOV [R%d] R%d\n",r0,r1);
								freeReg();
								freeReg();
								return;
		
		case ADD			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case SUB			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"SUB R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case MUL			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MUL R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case DIV			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"DIV R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case MOD			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MOD R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case GT				:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"GT R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case LT				:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"LT R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case GTE			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"GE R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case LTE			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"LE R%d R%d\n",r0,r1);
								freeReg();
								return r0;

		case EQ				:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"EQ R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case NE				:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"NE R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case And			:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MUL R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case Or				:	r0 = codeGenerate(root->Ptr1,Lhead);
								r1 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case Not			:	r0 = getReg();
								fprintf(fp,"MOV R%d %d\n",r0,1);
								
								r1 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"SUB R%d R%d\n",r0,r1);
								freeReg();
								return r0;
		
		case True			:
		
		case False			:	r0 = getReg();
								fprintf(fp,"MOV R%d %d\n",r0,root->VALUE);
								return r0;
		
		case IDFR			:	idtype = 0;
								r0 = Hlookup(root->NAME,Lhead);
								fprintf(fp,"MOV R%d [R%d]\n",r0,r0);
								return r0;
		
		case ARRAYIDFR		:	idtype = 0;
								r0 = Hlookup(root->NAME,Lhead);
								r1 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r0,r1);
								freeReg();
								fprintf(fp,"MOV R%d [R%d]\n",r0,r0);
								return r0;
		
		case NUM			:	r0 = getReg();
								fprintf(fp,"MOV R%d %d\n",r0,root->VALUE);
								return r0;
	}
}


/*	Hierarchical lookup for an identifier, first in local symbol table, then global table.
	Returns absolute location of the identifier in stack
	*/
int Hlookup(char *NAME,struct Lsymbol **Lhead)
{
	struct Lsymbol *lnode;
	struct Gsymbol *gnode;
	int r0,r1,type,bind;
	
	lnode = Llookup(NAME,Lhead);
	gnode = Glookup(NAME);
	
	if(lnode != NULL)
	{
		type = lnode->TYPE;
		bind = lnode->BINDING;
		
		if(type == INTGR || type == BOOL)
		{
			r0 = getReg();
			
			if(idtype == IDADDR)
				fprintf(fp,"MOV R%d [BP]\n",r0);
			else if (idtype == 0)
				fprintf(fp,"MOV R%d BP\n",r0);
			
			r1 = getReg();
			fprintf(fp,"MOV R%d %d\n",r1,bind);
			fprintf(fp,"ADD R%d R%d\n",r0,r1);
		}
		
		else if(type == INTGRALIAS || type == BOOLALIAS)
		{
			r0 = getReg();
			fprintf(fp,"MOV R%d BP\n",r0);
			
			r1 = getReg();
			fprintf(fp,"MOV R%d %d\n",r1,bind);
			
			fprintf(fp,"ADD R%d R%d\n",r1,r0);
			fprintf(fp,"MOV R%d [R%d]\n",r0,r1);
		}
		
		freeReg();
	}
	
	else if(gnode != NULL)
	{
		bind = gnode->BINDING;
		r0 = getReg();
		fprintf(fp,"MOV R%d %d\n",r0,bind);
	}
	
	return r0;
}

void pushGlobalVar()
{
	struct Gsymbol *ptr = Ghead;
	int i,r0;
	
	while(ptr)
	{	
		if(ptr->SIZE >= 0)
		{
			i = 0;
			do{
				r0 = getReg();
				fprintf(fp,"PUSH R%d\n",r0);
				freeReg();
			}while(++i < ptr->SIZE);
		}
		
		ptr = ptr->NEXT;
	}
}

void pushLocalVar(struct Lsymbol **Lhead)
{
	int r0;
	struct Lsymbol *ptr = *Lhead;
	
	while(ptr && ptr->BINDING>0)
	{
		r0 = getReg();
		fprintf(fp,"PUSH R%d\n",r0);
		freeReg();
		
		ptr = ptr->NEXT;
	}
}


/*	Push all registers that are currently in use into stack
	*/
int pushAllRegs()
{
	int i = regcnt;
	
	while(i >= 0)
	{
		fprintf(fp,"PUSH R%d\n",i);
		freeReg();
		i--;
	}
	
}


/*	Push function call parameters in reverse onto the stack
	*/
void pushCallParams(Tnode *root,struct Lsymbol **Lhead)
{
	int r0;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	pushCallParams(root->Ptr2,Lhead);
								pushCallParams(root->Ptr1,Lhead);
								break;
		
		case FUNCPARAM		:	idtype = 0;
								r0 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"PUSH R%d\n",r0);
								freeReg();
								break;
	}
}

void popAllLocal(struct Lsymbol **Lhead)
{
	struct Lsymbol *ptr = *Lhead;
	
	while(ptr && ptr->BINDING>0)
	{
		fprintf(fp,"POP R0\n");
		ptr = ptr->NEXT;
	}
}

void popAllArgs(ArgStruct *Ahead)
{
	ArgStruct *ptr = Ahead;
	int r0;
	
	while(ptr)
	{
		r0 = getReg();
		fprintf(fp,"POP R%d\n",r0);
		freeReg();
		ptr = ptr->NEXT;
	}
}

void restoreRegs(int cnt)
{
	int i,r;
	
	i = 0;
	regcnt = -1;
	
	while(i <= cnt)
	{
		r = getReg();
		fprintf(fp,"POP R%d\n",r);
		i++;
	}
	
	getReg();
}
