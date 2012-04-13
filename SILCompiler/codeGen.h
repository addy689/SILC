/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#ifndef CODEGEN_H
#define CODEGEN_H

#include "compilerLib.h"

void funcCodeGen(Tnode *root);
void argInstallLocal(ArgStruct *Arghead,struct Lsymbol **Lhead);
int codeGenerate(Tnode *root,struct Lsymbol **Lhead);
int lookupBinding(char *NAME,struct Lsymbol **Lhead);
int pushAllRegs();
void pushCallParams(Tnode *root,struct Lsymbol **Lhead);
void popAllLocal(struct Lsymbol **Lhead);
void popAllArgs(ArgStruct *Ahead);
void restoreRegs(int cnt);
void pushLocalVar(struct Lsymbol **Lhead);
void pushGlobalVar();

#endif
