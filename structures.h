#ifndef __STRUCTS__
#define __STRUCTS__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define EPSILON '.'
#define DEBUG(x)

//Stack Pointer Structure
//-----------------------
typedef struct no *stack;
typedef struct no
{
    void *info;
    stack next;
} no;

//Set structure
//-------------
typedef struct set
{
    int info;
    struct set *next;
} set;

//Link structure
//--------------
typedef struct link
{
    char symbol;
    int state;
    struct link *next;
} link;


//NFA - Nondeterministic Finite Automata Structure
//------------------------------------------------
//  [ ] nStates
//  transitions(array):
//  -------------------
//         0 [ ]-> [symbol|state|next] -> [.|.|/]
//         1 [ ]-> ...
//           ...
//  nStates-1[ ]-> ...
//  0 = initial state, nStates-1 = final state

typedef struct
{
    int nStates;
    link **transitions;
} nfa;


// Dfa state structure
//--------------------
typedef struct dfaState
{
    int final;
    int initial;
    set *nfaStates;
    struct dfaState *next;
} dfaState;


//DFA = Deteministic Finite Automata Structure
//--------------------------------------------
//  [ ] nSymbols
//  [s_0,s_1,...s_{nSymbols-1}] sigma (vocabulary)
//  [ ] nStates
//  states -> [final|initial|{N_1,N_2,...N_n}|next]->...
//       where N_i = set of corresponding nfa states
//  transitions (state * nSymbols + i_symbol):
//  ----------------------------------------
//               0   1   2  .... nSymbols-1
//             +---+---+---------+---
//           0 | x   x             x
//           1 | x   x             x
//     (...)
//   nStates-1 | x   x             x

typedef struct
{
    int nSymbols;
    char *sigma;
    int nStates;
    dfaState *states;
    int *transitions;
} dfa;
#endif