/*-----------------------------------------------------------------------
 * redfa - Converts Regex to minimized deterministic finite automata
 * Using | to union, . (not digited) to concat and * to kleene closure
 * By Luiz Eduardo da Silva - 2019
 * 
 * This file is part of redfa.
 *
 * redfa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * redfa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with redfa.  If not, see <https://www.gnu.org/licenses/>
 *-----------------------------------------------------------------------*/

#include "dfa.h"
#include "stack.h"
#include "set.h"

//DFA Automata Functions
//----------------------
int insertState(dfaState **D, set *S, int final, int initial)
{
    dfaState *st, *cur = *D, *prev = NULL;
    int pos = 0;
    while (cur)
    {
        prev = cur;
        if (equalSet(cur->stateSet, S))
            return pos;
        cur = cur->next;
        pos++;
    }
    st = malloc(sizeof(dfaState));
    st->final = final;
    st->initial = initial;
    st->stateSet = S;
    st->next = NULL;
    if (prev)
        prev->next = st;
    else
        *D = st;
    return pos;
}

set *eClose(nfa *N, int state)
{
    int *stack = malloc(sizeof(int) * N->nStates);
    int *visited = malloc(sizeof(int) * N->nStates);
    int top = -1;
    set *S = NULL;
    int i;
    for (i = 0; i < N->nStates; i++)
        visited[i] = 0;
    insertSet(&S, state);
    stack[++top] = state;
    // Deep first search - epsilon/lambda transitions
    while (top >= 0)
    {
        state = stack[top--];
        struct link *L = N->transitions[state];
        while (L)
        {
            if (L->symbol == EPSILON && !visited[L->state])
            {
                visited[L->state] = 1;
                insertSet(&S, L->state);
                stack[++top] = L->state;
            }
            L = L->next;
        }
    }
    free (stack);
    free (visited);
    return S;
}

set *getVocabulary(nfa *N)
{
    set *S = NULL;
    int i;
    for (i = 0; i < N->nStates; i++)
    {
        link *L = N->transitions[i];
        while (L)
        {
            insertSet(&S, L->symbol);
            L = L->next;
        }
    }
    return S;
}

set *delta(nfa *N, set *S, char symbol)
{
    set *state = NULL;
    while (S)
    {
        link *L = N->transitions[S->info];
        while (L)
        {
            if (L->symbol == symbol)
                insertSet(&state, L->state);
            L = L->next;
        }
        S = S->next;
    }
    return state;
}

// int statePosition(dfaState *D, set *state)
// {
//     int pos = 0;
//     while (D)
//     {
//         set *st = D->stateSet;
//         if (equalSet(st, state))
//             return pos;
//         pos++;
//         D = D->next;
//     }
//     return -1;
// }

typedef struct noDelta
{
    int in;
    char symb;
    int out;
    struct noDelta *next;
} noDelta;

void insertDelta(noDelta **L, int in, char symb, int out)
{
    noDelta *n = malloc(sizeof(noDelta));
    n->in = in;
    n->symb = symb;
    n->out = out;
    n->next = *L;
    *L = n;
}

dfa *nfaToDfa(nfa *N)
{
    struct noPos
    {
        int id;
        set *state;
    } *pos; // to store id and corresponding state
    dfa *D = malloc(sizeof(dfa));
    noDelta *L = NULL; // to store list of transitions
    int nStates = 0;   // to count number of DFA states
    set *sigma = getVocabulary(N);
    set *state = eClose(N, 0); // to calculate initial DFA state
    stack Stack = NULL;
    dfaState *stateList = NULL;
    int final, in, out;
    sigma = sigma->next; // to skip EPSILON symbol
    final = inSet(N->nStates - 1, state);
    insertState(&stateList, state, final, 1); // to insert initial state
    nStates++;  // to count initial state
    pos = malloc (sizeof (struct noPos));
    pos->id = 0;
    pos->state = state;
    push(&Stack, pos);
    while (Stack)
    {
        pos = pop(&Stack);
        state = pos->state;
        in = pos->id;
        set *S = sigma;
        free(pos);
        //in = statePosition(stateList, state);       
        while (S)
        {
            char symbol = S->info;
            set *newState = delta(N, state, symbol);
            set *newUnion = NULL;
            while (newState)
            {
                set *tmp = newState;
                set *newEclose = eClose(N, newState->info);
                unionSet(&newUnion, newEclose);
                newState = newState->next;
                disposeSet(newEclose);
                free (tmp);
            }
            final = inSet(N->nStates - 1, newUnion);
            out = insertState(&stateList, newUnion, final, 0);
            insertDelta(&L, in, symbol, out);
            if (out >= nStates)
            {
                pos = malloc (sizeof (struct noPos));
                pos->id = out;
                pos->state = newUnion;
                nStates++;
                push(&Stack, pos);
            }
            S = S->next;
        }
    }
    // Fill DFA structure
    D->states = stateList;
    D->nSymbols = lengthSet(sigma);
    D->sigma = malloc(D->nSymbols * sizeof(char) + 1);
    int i = 0;
    while (sigma)
    {
        set *tmp = sigma;
        D->sigma[i++] = sigma->info;
        sigma = sigma->next;
        free (tmp);
    }
    D->sigma[D->nSymbols] = 0;
    D->nStates = nStates;
    D->transitions = malloc(D->nStates * D->nSymbols * sizeof(int));
    while (L)
    {
        int j;
        noDelta *tmp = L;
        for (j = 0; D->sigma[j] && L->symb != D->sigma[j]; j++)
            ;
        D->transitions[L->in * D->nSymbols + j] = L->out;
        L = L->next;
        free(tmp);
    }
    return D;
}

void disposeDfaAutomata(dfa *D)
{
    int i;
    if (!D) return;
    dfaState *S = D->states;
    while (S)
    {
        set *state = S->stateSet;
        disposeSet(state);
        S = S->next;
    }
    free(D->sigma);
    free(D->transitions);
    free(D);
}

dfa *minimize(dfa *D)
{
    dfa *Dmin = malloc(sizeof(dfa));
    int nStates = D->nStates;
    int nSymbols = D->nSymbols;
    dfaState *L, *st = D->states;
    set *state;
    int *transitions = malloc(nStates * sizeof(int));
    int *groups = malloc(nStates * sizeof(int));
    int *diff = malloc(nStates * sizeof(int));
    int i, j, k, changed, countGroups, nGroups, nDiff, allFinal;
    i = 0;
    allFinal = 1;
    while (st)
    {
        if (!st->final)
           allFinal = 0;
        groups[i++] = st->final;
        st = st->next;
    }
    if (allFinal) {  // All states are final
        Dmin->nStates = 1;
        Dmin->nSymbols = nSymbols;
        Dmin->sigma = malloc(nSymbols * sizeof(char) + 1);
        for (i = 0; i < nSymbols; i++)
            Dmin->sigma[i] = D->sigma[i];
        Dmin->sigma[i] = 0;
        Dmin->transitions = malloc(Dmin->nSymbols * sizeof(int));
        state = NULL;
        for (i = 0; i < D->nStates; i++)
            insertSet (&state, i);
        st = NULL;        
        insertState (&st, state, 1, 1);
        Dmin->states = st;
        for (i = 0; i < nSymbols; i++)
            Dmin->transitions[i] = 0; 
        free(transitions);
        free(groups);
        free(diff);
        return Dmin;       
    }
    changed = 1;
    nGroups = 2; //nonfinal == 0, final == 1
    while (changed && nGroups < nStates)
    {
        changed = 0;
        //Group separation
        for (i = 0; i < nStates; i++)
        {
            transitions[i] = 0;
            for (j = 0; j < nSymbols; j++)
            {
                int group = groups[D->transitions[i * nSymbols + j]];
                transitions[i] = 10 * transitions[i] + group;
            }
        }
        //Group division
        countGroups = nGroups;
        for (k = 0; k < nGroups; k++)
        {
            nDiff = 0;
            for (i = 0; i < nStates; i++)
            {
                if (groups[i] == k)
                {
                    int ehDiff = 1;
                    for (j = 0; j < nDiff; j++)
                        if (diff[j] == transitions[i])
                            ehDiff = 0;
                    if (ehDiff)
                        diff[nDiff++] = transitions[i];
                }
            }
            if (nDiff > 1)
            {
                changed = 1;
                for (j = 1; j < nDiff; j++, countGroups++)
                    for (i = 0; i < nStates; i++)
                        if (groups[i] == k && diff[j] == transitions[i])
                            groups[i] = countGroups;
            }
        }
        nGroups = countGroups;
    }
    Dmin->nStates = nGroups;
    Dmin->nSymbols = nSymbols;
    Dmin->sigma = malloc(nSymbols * sizeof(char) + 1);
    for (i = 0; i < nSymbols; i++)
        Dmin->sigma[i] = D->sigma[i];
    Dmin->sigma[i] = 0;
    Dmin->transitions = malloc(Dmin->nStates * Dmin->nSymbols * sizeof(int));
    st = NULL;
    for (i = 0; i < nGroups; i++)
    {
        int final = 0;
        int initial = 0;
        L = D->states;
        j = 0;
        state = NULL;
        while (L)
        {
            if (groups[j] == i)
            {
                insertSet(&state, j);
                if (L->final)
                    final = 1;
                if (!j)
                    initial = 1;
            }
            L = L->next;
            j++;
        }
        insertState(&st, state, final, initial);
    }
    Dmin->states = st;
    for (i = 0; i < nGroups; i++)
        for (j = 0; j < nSymbols; j++)
        {
            for (k = 0; k < nStates && groups[k] != i; k++)
                ;
            Dmin->transitions[i * nSymbols + j] = groups[D->transitions[k * nSymbols + j]];
        }
    free(transitions);
    free(groups);
    free(diff);
    return Dmin;
}

void showDfaStates(dfaState *D)
{
    printf("[");
    while (D)
    {
        if (D->initial)
            printf(">");
        printSet(stdout, D->stateSet, 'i');
        if (D->final)
            printf("*");
        D = D->next;
        if (D)
            printf(",");
    }
    printf("]\n");
}

void displayDfaAutomata(dfa *D, char *regex)
{
    int i, j;
    printf("\nDFA : %s\n", regex);
    printf("------");
    for (i = 0; regex[i]; i++)
        printf("-");
    printf("\n");
    printf("nSymbols = %d\n", D->nSymbols);
    printf("Symbols  = \"%s\"\n", D->sigma);
    printf("nStates  = %d\n", D->nStates);
    printf("States   = ");
    showDfaStates(D->states);
    printf("Transitions:\n");
    printf("%4c ", ' ');
    for (i = 0; i < D->nSymbols; i++)
        printf("%4c", D->sigma[i]);
    printf("\n");
    for (i = 0; i < D->nStates; i++)
    {
        printf("%4d:", i);
        for (j = 0; j < D->nSymbols; j++)
            printf("%4d", D->transitions[i * D->nSymbols + j]);
        printf("\n");
    }
}

void saveDfaDotFile(dfa *A, char *name, char *regex, int showSetState)
{
    int i, j, initial;
    FILE *file = fopen(name, "wt");
    dfaState *st = A->states;
    fprintf(file, "digraph DFA {\n\trankdir=LR\n");
    fprintf(file, "\tlabel = \"regex=%s\"\n", regex);    
    fprintf(file, "\tinitial [shape=point]\n");
    i = 0;
    while (st)
    {
        if (!showSetState) {
           if (st->final)
              fprintf(file, "\ts%d [shape=doublecircle]\n", i);
           else
              fprintf(file, "\ts%d [shape=circle]\n", i);
        } else {
           if (st->final) {
              fprintf(file, "\ts%d\t[shape = doublecircle label=<\n", i);
           }
           else
              fprintf(file, "\ts%d\t[shape = circle label=<\n", i);
           fprintf(file, "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n");
           fprintf(file, "\t\t<tr> <td>  <font point-size=\"20\">s<SUB>%d</SUB></font></td> </tr>\n",i);
           fprintf(file, "\t\t<tr> <td>  <font point-size=\"8\">");
           printSet (file, st->stateSet, 'i');
           fprintf(file, "</font></td> </tr>\n\t\t</TABLE>>]\n");
        }
        if (st->initial)
            initial = i;
        st = st->next;
        i++;
    }
    fprintf(file, "\tinitial -> s%d\n", initial);
    for (i = 0; i < A->nStates; i++)
        for (j = 0; j < A->nSymbols; j++)
            fprintf(file, "\ts%d -> s%d [label = %c]\n", i, A->transitions[i * A->nSymbols + j], A->sigma[j]);
    fprintf(file, "}\n");
    fclose(file);
}
