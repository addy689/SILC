#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define ASSIGN 5
#define ARRAYASSIGN 13
#define NUM 6
#define IDFR 7
#define RD 8
#define ARRAYRD 9
#define WRIT 10
#define ARRAYIDFR 11
#define CONTINUE 12
#define IDFRDECL 14
#define ARRAYDECL 15
#define INTGR 16
#define BOOL 17
#define DATATYPE 18
#define DECLSTATEMENT 19

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
			struct tnode *Ptr1,*Ptr2,*Ptr3;
	}Tnode;

Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3);

void compile(Tnode *declroot,Tnode *stroot);
void globalInstall(Tnode *root);
struct Gsymbol *gLookup(char *NAME); // Look up for a global identifier
void gInstall(char *NAME,int TYPE,int SIZE); // Installation
void gAllocate();//Allocateing memory to variables in symbol table
struct Gsymbol *checkIdentDecl(char *NAME);
void semanticCheck(Tnode *root);

int ex(Tnode *root);

Tnode *tempnode;
struct Gsymbol *gtemp;
int var,error;
