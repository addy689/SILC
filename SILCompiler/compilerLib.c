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
	
	if(module == CODEGEN)
	{
		if(SIZE >= 0)
		{
			Gnode->BINDING = getPositiveLoc();
			
			for(i=0;i<SIZE-1;i++)
				getPositiveLoc();
		}
		
		else Gnode->BINDING = getLabel();
	}
	
	Gnode->NEXT = Ghead;
	Ghead = Gnode;
}


/*	Install all local declarations (identifiers) present in a function into that function's local symbol table
	*/
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
void LinstallBind(char *NAME,int TYPE,int VALUE,struct Lsymbol **Lhead)
{
	struct Lsymbol *Lnode = malloc(sizeof(struct Lsymbol));
	
	Lnode->NAME = malloc(sizeof(char) * (strlen(NAME)+1));
	strcpy(Lnode->NAME,NAME);
	
	Lnode->TYPE = TYPE;
	
	switch(module)
	{
		case INTERPRET		:	if(binding == NULL)
							{
								Lnode->LOCATION = malloc(sizeof(int));
								*Lnode->LOCATION = VALUE;
							}
							
							else Lnode->LOCATION = binding;	//For call by reference
							
							break;
		
		case CODEGEN			:	
/*							if(idstatus = ARGUMENT)*/
/*							{*/
/*								if(idcall = CALLBYREF)*/
/*									Lnode->BINDING = getNegativeLoc();*/
/*								else Lnode->BINDING = address of calling function's identifier*/
/*							}*/
/*							else Lnode->BINDING = getPositiveLoc();*/
							break;
	}
	
	Lnode->NEXT = *Lhead;
	*Lhead = Lnode;
}


/*	Allocate memory to identifiers in the global symbol table
	*/
void Gallocate()
{
	struct Gsymbol *Gnode = Ghead;
	
	while(Gnode)
	{
		if(Gnode->SIZE > 0)
			Gnode->LOCATION = malloc(sizeof(int) * Gnode->SIZE);
		else if(Gnode->SIZE == 0)
			Gnode->LOCATION = malloc(sizeof(int));
		
		Gnode = Gnode->NEXT;
	}
}

int getPositiveLoc()
{
	++locpos;
}

int getNegativeLoc()
{
	++locneg;
}

int getReg()
{
	return ++regcnt;
}

void freeReg()
{
	--regcnt;
}

int getLabel()
{
	return ++labelcnt;
}

void freeLabel()
{
	--labelcnt;
}


void printGlobal()
{
	struct Gsymbol *ptr = Ghead;
	char typ[10];
	char data[10];
	int ctr = 1;

	printf("\n---GLOBAL TABLE---\n");
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

	printf("\n---LOCAL TABLE---\n");
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
