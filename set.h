#ifndef __SETS__
#define __SETS__
#include "structures.h"

//list sort insert
void insertSet(set **, int);
void unionSet(set **, set *);
int inSet(int, set *);
int lengthSet(set *);
int equalSet(set *, set *);
void printSet(FILE *, set *, char);
void disposeSet(set *);

#endif