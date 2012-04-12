/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#ifndef CODEGEN_H
#define CODEGEN_H

#include "compilerLib.h"

void funcCodeGen(Tnode *root);
int argInstallLocal(ArgStruct *Arghead,struct Lsymbol **Lhead);
int codeGenerate(Tnode *root);
void pushLocalVar(struct Lsymbol **Lhead);
void pushGlobalVar();

#endif
