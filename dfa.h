#ifndef __DFA__
#define __DFA__
#include "structures.h"

//DFA Automata Functions
//----------------------
int insertState(dfaState **, set *, int, int);
set *eClose(nfa *, int);
set *getVocabulary(nfa *);
set *delta(nfa *, set *, char);
void showDfaStates(dfaState *);
//int statePosition(dfaState *, set *);
dfa *nfaToDfa(nfa *);
void displayDfaAutomata(dfa *, char *);
void disposeDfaAutomata(dfa *);
dfa *minimize(dfa *);
void saveDfaDotFile(dfa *, char *, char *, int);

#endif