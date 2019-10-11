#ifndef __NFA__
#define __NFA__

#include "structures.h"

//NFA Automata Functions
//----------------------
void insertLink(link **, char, int);
nfa *buildSymbol(char);
nfa *buildKleene(nfa *);
nfa *buildUnion(nfa *, nfa *);
nfa *buildConcat(nfa *, nfa *);
void disposeNfaAutomata(nfa *);
void displayNfaAutomata(nfa *, char *);
int isAlphabet(char);
nfa *regexToNfa(char *);
void saveNfaDotFile(nfa *, char *, char *);

#endif