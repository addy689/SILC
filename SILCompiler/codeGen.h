/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#ifndef CODEGEN_H
#define CODEGEN_H

#include "compilerLib.h"

int funcCodeGen(Tnode *root);
int funcBodyCodeGen(Tnode *root,struct Lsymbol **Lhead);
int argInstallLocal(ArgStruct *Arghead,struct Lsymbol **Lhead);
int codeGenerate(Tnode *root);

#endif

