/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#ifndef INTERPRET_H
#define INTERPRET_H

#include "compilerLib.h"

int interpreter(Tnode *root,struct Lsymbol **Lhead);
int evalBody(Tnode *root,struct Lsymbol **Lhead);
int funcParamInstall(Tnode *root,struct Lsymbol **Lhead,struct Lsymbol **Ltable);
Tnode *searchFunc(char *NAME,Tnode *root);

#endif
