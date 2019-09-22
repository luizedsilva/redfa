#ifndef DEBUG
#define DEBUG(x)
#endif

//DFA Automata Functions
//----------------------
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
        printSet(D->nfaStates, 'i');
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
        if (equalSet(D->nfaStates, state))
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

dfaState *getDfaStates(nfa *N, int *num)
{
    set *sigma = getVocabulary(N), *state = eClose(N, 0);
    stack Stack = NULL;
    dfaState *D = NULL;
    int final = inSet(N->nStates - 1, state);
    DEBUG(printSet(state, 'i');
          printf(" final = %d\n", final);)
    sigma = sigma->next; // skip EPSILON symbol
    insertState(&D, state, final);
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
                unionSet(&newUnion, newEclose);
                newState = newState->next;
                disposeSet(newEclose);
            }
            disposeSet(newState);
            if (statePosition(D, newUnion) == -1)
            {
                final = inSet(N->nStates - 1, newUnion);
                DEBUG(printSet(newUnion, 'i');
                      printf(" final = %d\n", final);)
                insertState(&D, newUnion, final);
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

void displayDfaAutomata(dfa *D)
{
    int i, j;
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

void saveDfaDotFile(dfa *A, char *name)
{
    int i, j;
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
        st = st->next;
        i++;
    }
    fprintf(file, "\tinitial -> s0\n");
    for (i = 0; i < A->nStates; i++)
        for (j = 0; j < A->nSymbols; j++)
            fprintf(file, "\ts%d -> s%d [label = %c]\n", i, A->transitions[i * A->nSymbols + j], A->sigma[j]);
    fprintf(file, "}\n");
    fclose(file);
}
