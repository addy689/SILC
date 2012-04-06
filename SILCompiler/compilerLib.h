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

typedef struct tnode {
			int TYPE;
			int NODETYPE;
			char *NAME;
			int VALUE;
			int LINE;
			struct tnode *ArgList;
			struct tnode *Ptr1,*Ptr2,*Ptr3;
	}Tnode;


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

Tnode *TreeCreate(int TYPE,int NODETYPE,char *NAME,int VALUE,Tnode *ArgList,Tnode *Ptr1,Tnode *Ptr2,Tnode *Ptr3,int LINE);

void compile(Tnode *gdeclroot,Tnode *fdefroot,Tnode *mainroot);

void globalInstall(Tnode *root);

int funcSemanticCheck(Tnode *root);
void argLocalInstall(Tnode *root,struct Lsymbol **Lhead);
int funcTypeCheck(Tnode *root);
int funcArgCheck(Tnode *root);
void checkFuncDecl(int LINE);
void localInstall(Tnode *root,struct Lsymbol **Lhead);
int bodySemanticCheck(Tnode *root,struct Lsymbol **Lhead);

ArgStruct *argLookup(char *NAME,ArgStruct *HEAD);
void argInstall(char *NAME,int TYPE);

struct Gsymbol *Glookup(char *NAME);	//Look up for a global identifier
void Ginstall(char *NAME,int TYPE,int SIZE,ArgStruct *ARGLIST); // Installation
void gAllocate();	//Allocating memory to variables in symbol table

struct Lsymbol *Llookup(char *NAME,struct Lsymbol **Lhead);
void Linstall(char* NAME, int TYPE,struct Lsymbol **Lhead);

int ex(Tnode *root,struct Lsymbol **Lhead);

void helplocal(Tnode *root,struct Lsymbol *lnode,int t1);
void helpglobal(Tnode *root,struct Gsymbol *gnode,int t1);
void printGlobal(struct Lsymbol **HEAD);

Tnode *tempnode,*decnode,*argnode;
struct Gsymbol *gtemp;
ArgStruct *Arghead,*Argrear;
int var,error,line,functype,entry;
