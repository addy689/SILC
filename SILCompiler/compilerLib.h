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

struct Gsymbol {
	char *NAME; // Name of the Identifier

	int TYPE; // TYPE can be INTEGER or BOOLEAN
	/***The TYPE field must be a TypeStruct if user defined types are allowed***/
	
	int SIZE; // Size field for arrays
	
	int VALUE;
	
	int *BINDING; // Address of the Identifier in Memory

//	ArgStruct *ARGLIST; // Argument List for functions
//	/***Argstruct must store the name and type of each argument ***/

	struct Gsymbol *NEXT; // Pointer to next Symbol Table Entry */

}*Ghead;


typedef struct tnode {
			int TYPE;
			int NODETYPE;
			char *NAME;
			int VALUE;
			int LINE;
			struct tnode *Ptr1,*Ptr2,*Ptr3;
	}Tnode;

Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3,int LINE);

void compile(Tnode *declroot,Tnode *stroot);
void globalInstall(Tnode *root);
struct Gsymbol *gLookup(char *NAME); // Look up for a global identifier
void gInstall(char *NAME,int TYPE,int SIZE); // Installation
void gAllocate();//Allocating memory to variables in symbol table
struct Gsymbol *checkIdentDecl(char *NAME,int LINE);
int semanticCheck(Tnode *root);

int ex(Tnode *root);

Tnode *tempnode;
struct Gsymbol *gtemp;
int var,error,line;
