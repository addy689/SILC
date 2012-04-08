/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

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
#define DATATYPE 33

#define IDARG 34
#define ARGSTATEMENT 35
#define FUNCDECL 36

#define FUNCBLOCK 37
#define MAINBLOCK 38
#define RET 39
#define FUNCCALL 40
#define FUNCPARAM 41
#define IDALIASARG 42
#define IDADDR 43



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
	int *BINDING;			//Address of the Identifier in Memory
	int LOCATION;
	ArgStruct *ARGLIST;		//Argument List for functions
	struct Gsymbol *NEXT;	//Pointer to next Symbol Table Entry
}*Ghead;

//Node structure for Local Symbol Table
struct Lsymbol {
	char *NAME;				//Name of the Identifier
	int TYPE;				//TYPE can be INTEGER or BOOLEAN
	int *BINDING;			//Address of the Identifier in Memory
	struct Lsymbol *NEXT;	//Pointer to next Symbol Table Entry
}*Lhead;



/*############# CREATE AST NODE ################*/

//Create a node for Abstract Syntax Tree
Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *ArgList,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3,int LINE);



/*############## COMPILE ###############*/

//Compile the SIL source code using the constructed Abstract Syntax Tree 
void compile(Tnode *gdeclroot,Tnode *fdefroot,Tnode *mainroot);



/*############# GLOBAL SYMBOL TABLE INSTALLATION & SEMANTIC CHECKING ################*/

//Install all global declarations present in source program into a global symbol table
void globalInstall(Tnode *root);

//Install function arguments present in global declaration of function into an argument list
void argInstall(char *NAME,int TYPE);

//Check a function's definition for semantic errors
int funcSemanticCheck(Tnode *root);

//Installs function arguments into that function's local symbol table (with semantic checking)
void argLocalInstall(Tnode *root,struct Lsymbol **Lhead);

//Lookup a function argument in the function argument list
ArgStruct *argLookup(char *NAME,ArgStruct *HEAD);

//Check if function has global declaration, same return type as in global declaration, and no redefinitions
int funcTypeCheck(Tnode *root);

//Name and Type checking of function arguments against the global declaration
int funcArgCheck(Tnode *root);

//Check for globally declared functions that do not have a function definition
void checkFuncDecl(int LINE);

//Install all local declarations (identifiers) present in a function into that function's local symbol table
void localInstall(Tnode *root,struct Lsymbol **Lhead);

//Check function body for semantic errors (in accordance with the Sinmple Integer Language specification)
int bodySemanticCheck(Tnode *root,struct Lsymbol **Lhead);

//Check assignment statement ('variable' = 'expression') for type mismatch, given that 'variable' is a local variable
void checkLocalAssign(Tnode *root,struct Lsymbol *lnode,int t1);

//Check assignment statement ('variable' = 'expression') for type mismatch, given that 'variable' is a global variable
void checkGlobalAssign(Tnode *root,struct Gsymbol *gnode,int t1);

//Look up an identifier in the global symbol table
struct Gsymbol *Glookup(char *NAME);

//Install an identifier in the global symbol table
void Ginstall(char *NAME,int TYPE,int SIZE,ArgStruct *ARGLIST);
int getLoc();

//Look up an identifier in local symbol table (of a function)
struct Lsymbol *Llookup(char *NAME,struct Lsymbol **Lhead);

//Install an identifier in the local symbol table (of a function)
void Linstall(char* NAME, int TYPE,struct Lsymbol **Lhead);

//Allocate memory to identifiers in the global symbol table
void Gallocate();



/*############## INTERPRET ###############*/
int interpreter(Tnode *root,struct Lsymbol **Lhead);
void localDecInstall(Tnode *root,struct Lsymbol **Lhead);
void LinstallBind(char *NAME,int TYPE,int VALUE,struct Lsymbol **Lhead);
Tnode *searchFunc(char *NAME,Tnode *root);
int evalBody(Tnode *root,struct Lsymbol **Lhead);



/*############## CODE GENERATION ###############*/
int codeGenerate(Tnode *root);
int getReg();
void freeReg();
int getLabel();
void freeLabel();



/*############## DEBUGGING HELP ###############*/
void printLocal(struct Lsymbol **HEAD);
void printGlobal();



/*############## GLOBAL VARIABLE DECLARATIONS ###############*/
Tnode *tempnode,*decnode,*argnode,*funcroot,*mroot;
struct Gsymbol *gtemp;
ArgStruct *Arghead,*Argrear;
int var,error,line,functype,entry;
int *binding;
int loc,regcnt,labelcnt;
FILE *fp;
