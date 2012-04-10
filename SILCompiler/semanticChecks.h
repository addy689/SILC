/**
* Copyright (C) <2012> Addy Singh <addy689@gmail.com>
*/

#ifndef SEMANTIC_CHECKS
#define SEMANTIC_CHECKS

#include "compilerLib.h"

int funcSemanticCheck(Tnode *root);
void argLocalInstall(Tnode *root,struct Lsymbol **Lhead);
int funcTypeCheck(Tnode *root);
int funcArgCheck(Tnode *root);
void checkFuncDecl(int LINE);
void localInstall(Tnode *root,struct Lsymbol **Lhead);
void Linstall(char *NAME,int TYPE,struct Lsymbol **Lhead);
int bodySemanticCheck(Tnode *root,struct Lsymbol **Lhead);
void checkLocalAssign(Tnode *root,struct Lsymbol *lnode,int t1);
void checkGlobalAssign(Tnode *root,struct Gsymbol *gnode,int t1);

#endif
