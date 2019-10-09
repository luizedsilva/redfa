#ifndef __DFA__
#define __DFA__
#include "structures.h"

//DFA Automata Functions
//----------------------
int insertState(dfaState **D, set *S, int final, int initial);
set *eClose(nfa *N, int state);
set *getVocabulary(nfa *N);
set *delta(nfa *N, set *S, char symbol);
void showDfaStates(dfaState *D);
void printDelta(set *stateIn, char symbol, set *stateOut);
int statePosition(dfaState *D, set *state);
set *getState(dfaState *D, int n);
dfaState *getDfaStates(nfa *N, int *num);
dfa *nfaToDfa(nfa *N);
void displayDfaAutomata(dfa *D, char *regex);
void disposeDfaAutomata(dfa *D);
dfa *minimize(dfa *D);
void saveDfaDotFile(dfa *A, char *name, char*s);

#endif