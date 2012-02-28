#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define ASSGN 5
#define NUM 6
#define IDFR 7
#define RD 8
#define WRT 9
#define IDSEQ 10
#define STLIST 11
#define INTGR 12
#define BOOL 13

struct Gsymbol {
	char *NAME; // Name of the Identifier

	int TYPE; // TYPE can be INTEGER or BOOLEAN
	/***The TYPE field must be a TypeStruct if user defined types are allowed***/
	
	int SIZE; // Size field for arrays
	
	int VALUE;
	
	int BINDING; // Address of the Identifier in Memory

//	ArgStruct *ARGLIST; // Argument List for functions
//	/***Argstruct must store the name and type of each argument ***/

	struct Gsymbol *NEXT; // Pointer to next Symbol Table Entry */
};

struct Tnode {
			int TYPE;
			int NODETYPE;
			char *NAME;
			int VALUE;
			struct Tnode *Ptr1,*Ptr2,*Ptr3;
			struct Gsymbol *Gentry; // For global identifiers/functions
	};

struct Tnode* allocateNode(int,int,char *,int,struct Tnode *,struct Tnode *,struct Tnode *);

struct Gsymbol *Gsymlist;
struct Gsymbol *Glookup(char *); // Look up for a global identifier
void Ginstall(char *NAME,int TYPE,int SIZE); // Installation

int ex(struct Tnode *);
