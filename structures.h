//Link structure
//--------------
typedef struct link
{
    char symbol;
    int state;
    struct link *next;
} link;

void insertLink(link **L, char symbol, int state)
{
    struct link *n = malloc(sizeof(link));
    n->state = state;
    n->symbol = symbol;
    n->next = *L;
    *L = n;
}

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

void insertState(dfaState **D, set *S, int final, int initial)
{
    dfaState *st, *cur = *D, *prev = NULL;
    while (cur)
    {
        prev = cur;
        if (equalSet(cur->nfaStates, S))
        {
            return;
        }
        cur = cur->next;
    }
    st = malloc(sizeof(dfaState));
    st->final = final;
    st->initial = initial;
    st->nfaStates = S;
    if (prev)
        prev->next = st;
    else
        *D = st;
}

//DFA = Deteministic Finite Automata Structure
//--------------------------------------------
//  [ ] nSymbols
//  [s_0,s_1,...s_{nSymbols-1}] sigma (vocabulary)
//  [ ] nStates
//  states -> [final|{0,1,2}|next]->...
//  transitions (state * nSymbols + i_symbol):
//  ----------------------------------------
//               0   1   2  .... nSymbols-1
//             +---+---+---------+---
//           0 | x   x             x
//           1 | x   x             x
//     (...)
//   nStates-1 | x   x             x
//  Initial state = first of nfaStates list

typedef struct
{
    int nSymbols;
    char *sigma;
    int nStates;
    dfaState *states;
    int *transitions;
} dfa;
