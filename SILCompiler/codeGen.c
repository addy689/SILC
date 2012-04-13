/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "codeGen.h"


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
								
								regcnt = -1;
								pushLocalVar(&Lhead);
								codeGenerate(root->Ptr3,&Lhead);
								
								return;
		
		case MAINBLOCK		:	fprintf(fp,"START\n");
								fprintf(fp,"MOV SP -1\n");
								regcnt = -1;
								pushGlobalVar();
								
								fprintf(fp,"PUSH BP\n");
								fprintf(fp,"MOV BP SP\n");
								
								locpos = -1;
								localDecInstall(root->Ptr1,&Lhead);
								
								pushLocalVar(&Lhead);
								
								codeGenerate(root->Ptr2,&Lhead);
								fprintf(fp,"HALT\n");
								
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
	int loc,r,r1,r2;
	int lbl1,lbl2;
	struct Gsymbol *gnode;
	struct Lsymbol *lnode;
	
	if(root==NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	codeGenerate(root->Ptr1,Lhead);
								codeGenerate(root->Ptr2,Lhead);
								
								return;
		
		case FUNCCALL		:	pushAllRegs();
								int rc = regcnt;
								pushCallParams(root->Ptr1,Lhead);
								
								r = getReg();
								fprintf(fp,"PUSH R%d\n",r);
								freeReg();
								
								gnode = Glookup(root->NAME);
								fprintf(fp,"CALL LABEL%d\n",gnode->BINDING);
								
								r = getReg();
								fprintf(fp,"MOV R%d SP\n",r);
								
								popAllArgs(gnode->ARGLIST);
								restoreRegs(rc);
								
								return r;
		
		//Call by Reference
/*		case IDADDR			:	if(*/
/*							r = lookupBinding(root->NAME,Lhead);*/
/*		*/
/*		case ARRAYIDADDR	:	gnode = Glookup(root->NAME);*/
/*								t = evalBody(root->Ptr1,Lhead);*/
/*								binding = gnode->LOCATION + t;*/
/*								*/
/*								return *binding;*/
		
		case RET			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = getReg();
								r = getReg();
								fprintf(fp,"MOV R%d BP\n",r2);
								fprintf(fp,"MOV R%d 2\n",r);
								fprintf(fp,"SUB R%d R%d\n",r2,r);
								fprintf(fp,"MOV [R%d] R%d\n",r2,r1);
								freeReg();
								freeReg();
								freeReg();
								
								popAllLocal(Lhead);
								fprintf(fp,"POP BP\n");
								fprintf(fp,"RET\n");
								
								return;
		
		case ITERATIVE		:	lbl1 = getLabel();
								lbl2 = getLabel();
								
								fprintf(fp,"LABEL%d:\n",lbl1);
								
								r = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"JZ R%d LABEL%d\n",r,lbl2);
								freeReg();
								
								codeGenerate(root->Ptr2,Lhead);
								
								fprintf(fp,"JMP LABEL%d\n",lbl1);
								fprintf(fp,"LABEL%d:\n",lbl2);
								
								return;
		
		case CONDITIONAL	:	r = codeGenerate(root->Ptr1,Lhead);
								
								lbl1 = getLabel();
								lbl2 = getLabel();
								fprintf(fp,"JZ R%d LABEL%d\n",r,lbl1);
								freeReg();
								
								codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"JMP LABEL%d\n",lbl2);
								
								fprintf(fp,"LABEL%d:\n",lbl1);
								codeGenerate(root->Ptr3,Lhead);
								
								fprintf(fp,"LABEL%d:\n",lbl2);
								
								return;
		
		case RD				:	r = getReg();
								fprintf(fp,"IN R%d\n",r);
								
								loc = lookupBinding(root->NAME,Lhead);
								fprintf(fp,"MOV [R%d] R%d\n",loc,r);
								freeReg();
								freeReg();
								
								return;
		
		case ARRAYRD		:	r = getReg();
								fprintf(fp,"IN R%d\n",r);
								
								loc = Glookup(root->NAME)->BINDING;
								r1 = codeGenerate(root->Ptr1,Lhead);
								
								r2 = getReg();
								fprintf(fp,"MOV R%d %d\n",r2,loc);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								
								fprintf(fp,"MOV [R%d] R%d\n",r1,r);
								freeReg();
								freeReg();
								
								return;
		
		case WRIT			:	r = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"OUT R%d\n",r);
								freeReg();
								
								return;
		
		case ASSIGN			:	loc = lookupBinding(root->NAME,Lhead);
								r = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"MOV [R%d] R%d\n",loc,r);
								freeReg();
								freeReg();
								
								return;
		
		case ARRAYASSIGN	:	loc = Glookup(root->NAME)->BINDING;
								r1 = codeGenerate(root->Ptr1,Lhead);
								
								r2 = getReg();
								fprintf(fp,"MOV R%d %d\n",r2,loc);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MOV [R%d] R%d\n",r1,r2);
								freeReg();
								freeReg();
								
								return ;
		
		case ADD			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case SUB			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"SUB R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case MUL			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MUL R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case DIV			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"DIV R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case MOD			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MOD R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case GT				:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"GT R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case LT				:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"LT R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case GTE			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"GE R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case LTE			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"LE R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;

		case EQ				:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"EQ R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case NE				:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"NE R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case And			:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"MUL R%d R%d\n",r1,r2);
								freeReg();
								return r1;
		
		case Or				:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = codeGenerate(root->Ptr2,Lhead);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								return r1;
		
		case Not			:	r1 = getReg();
								r2 = codeGenerate(root->Ptr1,Lhead);
								fprintf(fp,"MOV R%d %d\n",r1,1);
								fprintf(fp,"SUB R%d R%d\n",r1,r2);
								freeReg();
								return r1;
		
		case True			:
		
		case False			:	r = getReg();
								fprintf(fp,"MOV R%d %d \n",r,root->VALUE);
								
								return r;
		
		case IDFR			:	loc = lookupBinding(root->NAME,Lhead);
								
								r1 = getReg();
								fprintf(fp,"MOV R%d [R%d]\n",r1,loc);
								
								return r1;
		
		case ARRAYIDFR		:		r1 = getReg();
								r2 = codeGenerate(root->Ptr1,Lhead);
								loc = Glookup(root->NAME)->BINDING;
								fprintf(fp,"MOV R%d %d\n",r1,loc);
								fprintf(fp,"ADD R%d R%d\n",r2,r1);
								fprintf(fp,"MOV R%d [R%d]\n",r1,r2);
								freeReg();
								return r1;
		
		case NUM			:	r = getReg();
								fprintf(fp,"MOV R%d %d\n",r,root->VALUE);
								
								return r;
	}
}


/*	Hierarchical lookup for an identifier, first in local symbol table, then global table.
	Returns absolute location of the identifier in stack
	*/
int lookupBinding(char *NAME,struct Lsymbol **Lhead)
{
	struct Lsymbol *lnode;
	struct Gsymbol *gnode;
	int bind,r1,r2,r3,type;
	
	lnode = Llookup(NAME,Lhead);
	gnode = Glookup(NAME);
	
	if(lnode != NULL)
	{
		type = lnode->TYPE;
		bind = lnode->BINDING;
		
		r1 = getReg();
		fprintf(fp,"MOV R%d BP\n",r1);
		
		r2 = getReg();
		fprintf(fp,"MOV R%d %d\n",r2,bind);
		
		fprintf(fp,"ADD R%d R%d\n",r1,r2);
		freeReg();
		
		switch(type)
		{
			case INTGRALIAS		:
			case BOOLALIAS		:	r2 = getReg();
									fprintf(fp,"MOV R%d [BP]\n",r2);
									
									r3 = getReg();
									fprintf(fp,"MOV R%d [R%d]\n",r3,r1);
									
									fprintf(fp,"ADD R%d R%d\n",r2,r3);
									freeReg();
									
									fprintf(fp,"MOV R%d R%d\n",r1,r2);
									freeReg();
									
									break;
		}
		
		return r1;
	}
	
	else
	{
		r1 = getReg();
		fprintf(fp,"MOV R%d %d\n",r1,gnode->BINDING);
		
		return r1;
	}
}

void pushGlobalVar()
{
	struct Gsymbol *ptr = Ghead;
	int i,r;
	
	while(ptr)
	{	
		if(ptr->SIZE >= 0)
		{
			i = 0;
			do{
				r = getReg();
				fprintf(fp,"PUSH R%d\n",r);
				freeReg();
			}while(i++ < ptr->SIZE);
		}
	
		ptr = ptr->NEXT;
	}
}

void pushLocalVar(struct Lsymbol **Lhead)
{
	struct Lsymbol *ptr = *Lhead;
	int r;
	
	while(ptr)
	{
		r = getReg();
		fprintf(fp,"PUSH R%d\n",r);
		freeReg();
	
		ptr = ptr->NEXT;
	}
}


/*	Push all registers that are currently in use into stack
	*/
int pushAllRegs()
{
	int i = 0;
	
	while(i <= regcnt)
	{
		fprintf(fp,"PUSH R%d\n",i);
		i++;
	}
}


/*	Push function call parameters in reverse onto the stack
	*/
void pushCallParams(Tnode *root,struct Lsymbol **Lhead)
{
	int r1,r2;
	
	if(root == NULL)
		return;
	
	switch(root->NODETYPE)
	{
		case CONTINUE		:	pushCallParams(root->Ptr2,Lhead);
								pushCallParams(root->Ptr1,Lhead);
								break;
		
		case FUNCPARAM		:	r1 = codeGenerate(root->Ptr1,Lhead);
								r2 = getReg();
								fprintf(fp,"MOV R%d R%d\n",r2,r1);
								fprintf(fp,"PUSH R%d\n",r2);
								freeReg();
								freeReg();
								break;
	}
}

void popAllLocal(struct Lsymbol **Lhead)
{
	struct Lsymbol *ptr = *Lhead;
	
	while(ptr)
	{
		fprintf(fp,"POP R0\n");
		ptr = ptr->NEXT;
	}
}

void popAllArgs(ArgStruct *Ahead)
{
	ArgStruct *ptr = Ahead;
	
	while(ptr)
	{
		fprintf(fp,"POP R0\n");
		ptr = ptr->NEXT;
	}
}

void restoreRegs(int cnt)
{
	regcnt = cnt;
	
	int r = cnt;
	while(r >= 0)
	{
		fprintf(fp,"POP R%d\n",r);
		r--;
	}
}
