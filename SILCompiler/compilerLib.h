/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#ifndef COMPILERLIB_H
#define COMPILERLIB_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define MOD 5
#define GT 6
#define LT 7
#define GTE 8
#define LTE 9
#define EQ 10
#define NE 11
#define And 12
#define Or 13
#define Not 14
#define True 15
#define False 16
#define NUM 17
#define IDFR 18
#define ARRAYIDFR 19

#define ASSIGN 20
#define ARRAYASSIGN 21
#define RD 22
#define ARRAYRD 23
#define WRIT 24
#define CONDITIONAL 25
#define ITERATIVE 26

#define CONTINUE 27

#define DECLSTATEMENT 28
#define IDFRDECL 29
#define ARRAYDECL 30
#define INTGR 31
#define BOOL 32
#define INTGRALIAS 48
#define BOOLALIAS 49
#define DATATYPE 33
#define LOCAL 50
#define GLOBAL 51

#define IDARG 34
#define ARGSTATEMENT 35
#define FUNCDECL 36

#define FUNCBLOCK 37
#define MAINBLOCK 38
#define RET 39
#define FUNCCALL 40
#define FUNCPARAM 41
#define IDADDRARG 42
#define IDADDR 43
#define ARRAYIDADDR 44
#define ARGUMENT 45

#define INTERPRET 46
#define CODEGEN 47



/*############# STRUCTURE DECLARATIONS ################*/

//Node structure for Abstract Syntax Tree
typedef struct tnode {
			int TYPE;
			int NODETYPE;
			char *NAME;
			int VALUE;
			int LINE;
			struct tnode *ArgList;
			struct tnode *Ptr1,*Ptr2,*Ptr3;
	}Tnode;

//Node structure for Argument List
typedef struct argstruct {
		char *NAME;
		int TYPE;
		int FLAG;
		struct argstruct *NEXT;
	}ArgStruct;

//Node structure for Global Symbol Table
struct Gsymbol {
	char *NAME;				//Name of the Identifier
	int TYPE;				//TYPE can be INTEGER or BOOLEAN
	int SIZE;				//Size field for arrays
	int BINDING;			//Address of the Identifier from base address BP (for SIM code generation only)
	int *LOCATION;			//Address of the Identifier in Memory (for interpretation only)
	ArgStruct *ARGLIST;		//Argument List for functions
	struct Gsymbol *NEXT;	//Pointer to next Symbol Table Entry
}*Ghead;

//Node structure for Local Symbol Table
struct Lsymbol {
	char *NAME;				//Name of the Identifier
	int TYPE;				//TYPE can be INTEGER or BOOLEAN
	int BINDING;			//Address of the Identifier from base address BP (for SIM code generation only)
	int *LOCATION;			//Address of the Identifier in Memory (for interpretation only)
	struct Lsymbol *NEXT;	//Pointer to next Symbol Table Entry
}*Lhead;

//Node structure for holding the pointers to all AST function nodes
typedef struct funcstruct {
		Tnode *FUNCNODE;
		struct funcstruct *NEXT;
	}FuncStruct;



/*############# CREATE AST NODE ################*/

//Create a node for Abstract Syntax Tree
Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *ArgList,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3,int LINE);



/*############# GLOBAL SYMBOL TABLE INSTALLATION ################*/

//Install all global declarations present in source program into a global symbol table
void globalInstall(Tnode *root);

//Lookup a function argument in the function argument list
ArgStruct *argLookup(char *NAME,ArgStruct *HEAD);

//Install function arguments present in global declaration of function into an argument list
void argInstall(char *NAME,int TYPE);

//Look up an identifier in the global symbol table
struct Gsymbol *Glookup(char *NAME);

//Install an identifier in the global symbol table
void Ginstall(char *NAME,int TYPE,int SIZE,ArgStruct *ARGLIST);

//Allocate memory to identifiers in the global symbol table (ONLY in Interpretation)
void Gallocate();

//Returns appropriate type for identifier addresses
int returnAddrType(int TYPE);


/*############# AST FUNCTION NODES MANAGEMENT (for faster access to a function body in the AST) ################*/

//Lookup an AST function node in the function nodes list (having head *Funchead)
Tnode *funcLookup(char *NAME);

//Add an AST function node to the function nodes list (having head *Funclist)
void addToFuncList(Tnode *root);

//Install AST function node in function nodes list (having head *Funclist)
void funcInstall(Tnode *func);



/*############# LOCAL SYMBOL TABLE INSTALLATION ################*/

//Install all local declarations (identifiers) present in a function into that function's local symbol table
void localDecInstall(Tnode *root,struct Lsymbol **Lhead);

//Look up an identifier in local symbol table (of a function)
struct Lsymbol *Llookup(char *NAME,struct Lsymbol **Lhead);

//Install an identifier in the local symbol table (of a function)
void LinstallBind(char *NAME,int TYPE,int VALUE,struct Lsymbol **Lhead);



/*############# IDENTIFIER BINDING ALLOCATION ################*/
int getPositiveLoc();
int getNegativeLoc();


/*############# REGISTER ALLOCATION ################*/
int getReg();
void freeReg();


/*############# LABEL ALLOCATION (for functions, iterative constructs and conditional statements) ################*/
int getLabel();
void freeLabel();


/*############# DISPLAY GLOBAL & LOCAL SYMBOL TABLE ################*/
void printLocal(struct Lsymbol **HEAD);
void printGlobal();


/*############# GLOBAL VARIABLE DECLARATIONS ################*/
Tnode *tempnode,*decnode,*argnode,*funcroot,*mroot;
FuncStruct *Funchead;
struct Gsymbol *gtemp;
ArgStruct *Arghead,*Argrear;
int module,status,idstatus,idtype;
int var,error,line,functype,entry;
int *binding;
int locpos,locneg,regcnt,labelcnt;
FILE *fp;

#endif
