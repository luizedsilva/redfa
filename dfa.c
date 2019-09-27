#include "dfa.h"
#include "stack.h"
#include "set.h"

//DFA Automata Functions
//----------------------
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
    // Deep first search - epsilon transitions
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

void showDfaStates(dfaState *D)
{
    printf("[");
    while (D)
    {
        if (D->initial)
            printf(">");
        printSet(D->nfaStates, 'i');
        if (D->final)
            printf("*");
        D = D->next;
        if (D)
            printf(",");
    }
    printf("]\n");
}

void printDelta(set *stateIn, char symbol, set *stateOut)
{
    printf("delta(");
    printSet(stateIn, 'i');
    printf(",%c) = ", symbol);
    printSet(stateOut, 'i');
    puts("");
}

int statePosition(dfaState *D, set *state)
{
    int pos = 0;
    while (D)
    {
        set *st = D->nfaStates;
        if (equalSet(st, state))
            return pos;
        pos++;
        D = D->next;
    }
    return -1;
}

set *getState(dfaState *D, int n)
{
    int pos = 0;
    while (D)
    {
        if (pos == n)
            return D->nfaStates;
        pos++;
        D = D->next;
    }
}

int nStack(stack S)
{
    int n = 0;
    while (S)
    {
        n++;
        S = S->next;
    }
    return n;
}

dfaState *getDfaStates(nfa *N, int *num)
{
    set *sigma = getVocabulary(N), *state = eClose(N, 0);
    stack Stack = NULL;
    dfaState *D = NULL;
    int final = inSet(N->nStates - 1, state);
    DEBUG(printSet(state, 'i'); printf("%c\n", final ? '*' : ' ');)
    sigma = sigma->next; // skip EPSILON symbol
    insertState(&D, state, final, 1);
    push(&Stack, state);
    *num = 1;
    while (Stack)
    {
        state = pop(&Stack);
        set *S = sigma;
        while (S)
        {
            char symbol = S->info;
            set *newState = delta(N, state, symbol);
            set *newUnion = NULL;
            while (newState)
            {
                set *newEclose = eClose(N, newState->info);
                DEBUG(printf("Eclose="); printSet(newEclose, 'i'); printf("\n");)
                unionSet(&newUnion, newEclose);
                newState = newState->next;
                //disposeSet(newEclose);
            }
            //disposeSet(newState);
            DEBUG(printDelta(state, symbol, newUnion);)
            int pos = statePosition(D, newUnion);
            if (pos == -1)
            {
                final = inSet(N->nStates - 1, newUnion);
                DEBUG(printSet(newUnion, 'i'); printf("%c\n", final ? '*' : ' ');)
                insertState(&D, newUnion, final, 0);
                (*num)++;
                push(&Stack, newUnion);
            }
            S = S->next;
        }
    }
    return D;
}

dfa *nfaToDfa(nfa *N)
{
    dfa *D = malloc(sizeof(dfa));
    set *sigma = getVocabulary(N);
    dfaState *L;
    int i, j, row;
    L = getDfaStates(N, &(D->nStates));
    D->states = L;
    sigma = sigma->next; //skip EPSILON symbol
    D->nSymbols = lengthSet(sigma);
    D->sigma = malloc(D->nSymbols * sizeof(char) + 1);
    i = 0;
    while (sigma)
    {
        D->sigma[i++] = sigma->info;
        sigma = sigma->next;
    }
    D->sigma[D->nSymbols] = 0;
    D->transitions = malloc(D->nStates * D->nSymbols * sizeof(int));
    for (i = 0; i < D->nStates; i++)
    {
        set *state = L->nfaStates;
        for (j = 0; j < D->nSymbols; j++)
        {
            char symbol = D->sigma[j];
            set *newState = delta(N, state, symbol);
            set *newUnion = NULL;
            while (newState)
            {
                set *newEclose = eClose(N, newState->info);
                unionSet(&newUnion, newEclose);
                newState = newState->next;
                disposeSet(newEclose);
            }
            DEBUG(printDelta(state, symbol, newUnion);)
            disposeSet(newState);
            D->transitions[i * D->nSymbols + j] = statePosition(D->states, newUnion);
        }
        L = L->next;
    }
    return D;
}

void displayDfaAutomata(dfa *D, char *regex)
{
    int i, j;
    printf("DFA : %s\n", regex);
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
    {
        printf("%4c", D->sigma[i]);
    }
    printf("\n");
    for (i = 0; i < D->nStates; i++)
    {
        printf("%4d:", i);
        for (j = 0; j < D->nSymbols; j++)
            printf("%4d", D->transitions[i * D->nSymbols + j]);
        printf("\n");
    }
}

void disposeDfaAutomata(dfa *D)
{
    int i;
    dfaState *S = D->states;
    while (S)
    {
        set *state = S->nfaStates;
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
    int i, j, k, changed, countGroups, nGroups, nDiff;
    i = 0;
    while (st)
    {
        groups[i++] = st->final;
        st = st->next;
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

void saveDfaDotFile(dfa *A, char *name)
{
    int i, j, initial;
    FILE *file = fopen(name, "wt");
    dfaState *st = A->states;
    fprintf(file, "digraph DFA {\n\trankdir=LR\n");
    fprintf(file, "\tinitial [shape=point]\n");
    i = 0;
    while (st)
    {
        if (st->final)
            fprintf(file, "\ts%d [shape=doublecircle]\n", i);
        else
            fprintf(file, "\ts%d [shape=circle]\n", i);
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
