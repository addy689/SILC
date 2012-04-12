/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#include "codeGen.h"


void funcCodeGen(Tnode *root)
{
	struct Gsymbol *gnode;
	struct Lsymbol *Lhead;
	
	Lhead = NULL;
	
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
								
								fprintf("fp,Label%d:\n",gnode->BINDING);
								codeGenerate(root->Ptr3);
								
								return;
		
		case MAINBLOCK		:	fprintf(fp,"MOV SP -1");
								pushGlobalVar();
								
								fprintf(fp,"MOV BP SP");
								fprintf(fp,"PUSH BP");
								
								locpos = -1;
								localDecInstall(root->Ptr1,&Lhead);
								
								pushLocalVar(&Lhead);
								
								regcnt = 0;
								codeGenerate(root->Ptr2,&Lhead);
	}
}


/*	Installs function arguments into that function's local symbol table (using the argument list stored in global symbol table entry of function)
	*/
void argInstallLocal(ArgStruct *HEAD,struct Lsymbol **Lhead)
{
	ArgStruct *ptr = HEAD;
	
	idstatus = ARGUMENT;
	while(ptr)
	{
		LinstallBind(ptr->NAME,ptr->TYPE,Lhead);
		ptr = ptr->NEXT;
	}
}

void pushAllRegs()
{
	int i = 0;
	
	while(i <= regcnt)
	{
		fprintf(fp,"PUSH R%d\n",i);
		i++;
	}
}

void funcCodeGen(char *NAME)
{
	struct Tnode *node;
	
	
	
	fprintf("fp,Label%d:\n",gnode->BINDING);
	codeGenerate(node->);
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
		case CONTINUE		:	codeGenerate(root->Ptr1);
								codeGenerate(root->Ptr2);
								
								return;
		
		case FUNCCALL		:	pushAllRegs();
								
								pushCallParams(root->Ptr1,Lhead);
								
								CALL LABEL 1;
								
								return;
		
		//Call by Reference
		case IDADDR			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									binding = lnode->BINDING;
								else binding = gnode->BINDING;
								
								return *binding;
		
		case RET			:	return evalBody(root->Ptr1,Lhead);
		
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
								loc = Glookup(root->NAME)->BINDING;
								r = getReg();
								fprintf(fp,"IN R%d\n",r);
								fprintf(fp,"MOV [%d] R%d\n",loc,r);
								freeReg();
								
								return -1;
		
		case ARRAYRD		:	fprintf(fp,"\n*** ARRAY READ ***\n");
								r = getReg();
								fprintf(fp,"IN R%d\n",r);
								loc = Glookup(root->NAME)->BINDING;
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
								loc = Glookup(root->NAME)->BINDING;
								r = codeGenerate(root->Ptr1);
								fprintf(fp,"MOV [%d] R%d\n",loc,r);
								freeReg();
								
								return;
		
		case ARRAYASSIGN	:	fprintf(fp,"\n*** ARRAY ASSIGNMENT ***\n");
								loc = Glookup(root->NAME)->BINDING;
								r1 = codeGenerate(root->Ptr1);
								r2 = getReg();
								fprintf(fp,"MOV R%d %d\n",r2,loc);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								
								r = codeGenerate(root->Ptr2);
								fprintf(fp,"MOV [R%d] R%d\n",r1,r);
								freeReg();
								freeReg();
								
								return -1;
		
		case ADD			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case SUB			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"SUB R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case MUL			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"MUL R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case DIV			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"DIV R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case MOD			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"MOD R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case GT				:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"GT R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case LT				:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"LT R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case GTE			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"GE R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case LTE			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"LE R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;

		case EQ				:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"EQ R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case NE				:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"NE R%d R%d\n",r1,r2);
								freeReg();
								
								return r1;
		
		case And			:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"MUL R%d R%d\n",r1,r2);
								
								return r1;
		
		case Or				:	r1 = codeGenerate(root->Ptr1);
								r2 = codeGenerate(root->Ptr2);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								
								return r1;
		
		case Not			:	r1 = getReg();
								r2 = codeGenerate(root->Ptr1);
								fprintf(fp,"MOV R%d %d\n",r1,1);
								fprintf(fp,"SUB R%d R%d\n",r1,r2);
								
								return r1;
		
		case True			:
		
		case False			:	r = getReg();
								fprintf(fp,"MOV R%d %d \n",r,root->VALUE);
								
								return r;
		
		case IDFR			:	lnode = Llookup(root->NAME,Lhead);
								gnode = Glookup(root->NAME);
								
								if(lnode != NULL)
									return *lnode->BINDING;
								else return *gnode->BINDING;
								
								loc = Glookup(root->NAME)->BINDING;
								r1 = getReg();
								fprintf(fp,"MOV R%d [%d]\n",r1,loc);
								
								return r1;
		
		case ARRAYIDFR		:	r1 = codeGenerate(root->Ptr1);
								loc = Glookup(root->NAME)->BINDING;
								r2 = getReg();
								fprintf(fp,"MOV R%d %d\n",r2,loc);
								fprintf(fp,"ADD R%d R%d\n",r1,r2);
								fprintf(fp,"MOV R%d [R%d]\n",r2,r1);
								freeReg();
								
								return r2;
		
		case NUM			:	r = getReg();
								fprintf(fp,"MOV R%d %d \n",r,root->VALUE);
								
								return r;
	}
}

void pushGlobalVar()
{
	struct Gsymbol *ptr = Ghead;
	int i;
	
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

void pushLocalVar(&Lhead)
{
	struct Lsymbol *ptr = Lhead;
	
	while(ptr)
	{
		r = getReg();
		fprintf(fp,"PUSH R%d\n",r);
		freeReg();
	
		ptr = ptr->NEXT;
	}
}

/*	Push function call parameters in reverse
	*/
void pushCallParams(Tnode *root,struct Lsymbol **Lhead)
{
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
