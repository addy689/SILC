#include<stdio.h>
#include<stdlib.h>

#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define ASSGN 5
#define NUM 6
#define VRBL 7
#define RD 8
#define WRT 9
#define STLIST 10

struct Gsymbol {
	char *NAME; // Name of the Identifier

	int TYPE; // TYPE can be INTEGER or BOOLEAN
	/***The TYPE field must be a TypeStruct if user defined types are allowed***/
	
	int SIZE; // Size field for arrays
	
	int BINDING; // Address of the Identifier in Memory

//	ArgStruct *ARGLIST; // Argument List for functions
//	/***Argstruct must store the name and type of each argument ***/

	struct Gsymbol *NEXT; // Pointer to next Symbol Table Entry */
};

struct Tnode {
			int val;
			char op;
			int flag;
			int binding;
			struct Tnode *l,*r;
			struct Gsymbol *Gentry; // For global identifiers/functions
	};

struct Tnode* allocateNode(int,int,char,int);
int ex(struct Tnode *);

//REWRITE
void insertSymTable(int,int,int);
