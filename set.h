#ifndef __SETS__
#define __SETS__
#include "structures.h"

//list sort insert
void insertSet(set **S, int i);
void unionSet(set **A, set *B);
int inSet(int i, set *S);
int lengthSet(set *S);
int equalSet(set *A, set *B);
void printSet(FILE *, set *, char);
void disposeSet(set *S);

#endif