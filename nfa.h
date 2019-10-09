#ifndef __NFA__
#define __NFA__

#include "structures.h"

//NFA Automata Functions
//----------------------
void insertLink(link **L, char symbol, int state);
nfa *buildSymbol(char symbol);
nfa *buildKleene(nfa *A);
nfa *buildUnion(nfa *A, nfa *B);
nfa *buildConcat(nfa *A, nfa *B);
void disposeNfaAutomata(nfa *A);
void displayNfaAutomata(nfa *A, char *regex);
int isAlphabet(char c);
nfa *regexToNfa(char *regex);
void saveNfaDotFile(nfa *A, char *name, char*);

#endif