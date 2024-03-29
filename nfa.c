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

#include "structures.h"
#include "stack.h"
#include "set.h"

//NFA Automata Functions
//----------------------

void insertLink(link **L, char symbol, int state)
{
    link *n = malloc(sizeof(link));
    n->state = state;
    n->symbol = symbol;
    n->next = *L;
    *L = n;
}

nfa *buildSymbol(char symbol)
{
    nfa *A = malloc(sizeof(nfa));
    A->nStates = 2;
    A->transitions = malloc(2 * sizeof(link *));
    A->transitions[0] = NULL;
    A->transitions[1] = NULL;
    insertLink(A->transitions + 0, symbol, 1);
    return A;
}

nfa *buildKleene(nfa *A)
{
    int i;
    nfa *newA = malloc(sizeof(nfa));
    newA->nStates = A->nStates + 2;
    newA->transitions = malloc(newA->nStates * sizeof(link *));
    for (i = 0; i < newA->nStates; i++)
        newA->transitions[i] = NULL;
    insertLink(newA->transitions + 0, EPSILON, 1);
    insertLink(newA->transitions + 0, EPSILON, newA->nStates - 1);
    for (i = 0; i < A->nStates; i++)
    {
        link *L = A->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + i + 1, L->symbol, L->state + 1);
            L = L->next;
        }
    }
    insertLink(newA->transitions + newA->nStates - 2, EPSILON, newA->nStates - 1);
    insertLink(newA->transitions + newA->nStates - 1, EPSILON, 0);
    return newA;
}

nfa *buildUnion(nfa *A, nfa *B)
{
    int i;
    nfa *newA = malloc(sizeof(nfa));
    newA->nStates = A->nStates + B->nStates + 2;
    newA->transitions = malloc(newA->nStates * sizeof(link *));
    for (i = 0; i < newA->nStates; i++)
        newA->transitions[i] = NULL;
    insertLink(newA->transitions + 0, EPSILON, 1);
    insertLink(newA->transitions + 0, EPSILON, A->nStates + 1);
    for (i = 0; i < A->nStates; i++)
    {
        link *L = A->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + i + 1, L->symbol, L->state + 1);
            L = L->next;
        }
    }
    for (i = 0; i < B->nStates; i++)
    {
        link *L = B->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + A->nStates + i + 1, L->symbol, L->state + A->nStates + 1);
            L = L->next;
        }
    }
    insertLink(newA->transitions + A->nStates, EPSILON, newA->nStates - 1);
    insertLink(newA->transitions + newA->nStates - 2, EPSILON, newA->nStates - 1);
    return newA;
}

nfa *buildConcat(nfa *A, nfa *B)
{
    int i;
    nfa *newA = malloc(sizeof(nfa));
    newA->nStates = A->nStates + B->nStates + 2;
    newA->transitions = malloc(newA->nStates * sizeof(link *));
    for (i = 0; i < newA->nStates; i++)
    {
        newA->transitions[i] = NULL;
    }
    insertLink(newA->transitions + 0, EPSILON, 1);
    for (i = 0; i < A->nStates; i++)
    {
        link *L = A->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + i + 1, L->symbol, L->state + 1);
            L = L->next;
        }
    }
    insertLink(newA->transitions + A->nStates, EPSILON, A->nStates + 1);
    for (i = 0; i < B->nStates; i++)
    {
        link *L = B->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + A->nStates + i + 1, L->symbol, L->state + A->nStates + 1);
            L = L->next;
        }
    }
    insertLink(newA->transitions + newA->nStates - 2, EPSILON, newA->nStates - 1);
    return newA;
}

void disposeNfaAutomata(nfa *A)
{
    int i;
    if (!A) return;
    for (i = 0; i < A->nStates; i++)
    {
        link *L = A->transitions[i];
        while (L)
        {
            link *temp = L;
            L = L->next;
            free(temp);
        }
    }
    free(A->transitions);
    free(A);
}

void displayNfaAutomata(nfa *A, char *regex)
{
    int i;
    printf("\nNFA : %s\n", regex);
    printf("------");
    for (i = 0; regex[i]; i++)
       printf ("-");
    printf("\n");
    printf("nStates = %d\n", A->nStates);
    printf("Transitions:\n");
    for (i = 0; i < A->nStates; i++)
    {
        link *L = A->transitions[i];
        printf("\ts%-2d : ", i);
        while (L)
        {
            printf("[%2c|%2d|", L->symbol, L->state);
            L = L->next;
            if (L)
               printf(" =]=>");
            else
               printf(" /]");
        }
        printf("\n");
    }
}

int isAlphabet(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

nfa *regexToNfa(char *regex)
{
    stack P = NULL;
    nfa *A, *B, *C;
    int i;
    char c;
    for (i = 0; regex[i]; i++)
    {
        c = tolower(regex[i]);
        if (isAlphabet(c))
        {
            A = buildSymbol(c);
            push(&P, A);
        }
        if (c == '*')
        {
            A = pop(&P);
            B = buildKleene(A);
            push(&P, B);
            disposeNfaAutomata(A);
        }
        if (c == '.')
        {
            B = pop(&P);
            A = pop(&P);
            C = buildConcat(A, B);
            push(&P, C);
            disposeNfaAutomata(A);
            disposeNfaAutomata(B);
        }
        if (c == '|')
        {
            B = pop(&P);
            A = pop(&P);
            C = buildUnion(A, B);
            push(&P, C);
            disposeNfaAutomata(A);
            disposeNfaAutomata(B);
        }
    }
    return pop(&P);
}

void saveNfaDotFile(nfa *A, char *name, char *regex)
{
    int i;
    FILE *file = fopen(name, "wt");
    fprintf(file, "digraph NFA {\n\trankdir=LR\n");
    fprintf(file, "\tlabel = \"regex = %s\"\n", regex);
    fprintf(file, "\tinitial [shape=point]\n");
    for (i = 0; i < A->nStates - 1; i++)
        fprintf(file, "\ts%d [shape=circle]\n", i);
    fprintf(file, "\ts%d [shape=doublecircle]\n", A->nStates - 1);
    fprintf(file, "\tinitial -> s0\n");
    for (i = 0; i < A->nStates; i++)
    {
        struct link *L = A->transitions[i];
        while (L)
        {
            if (L->symbol == EPSILON)
                fprintf(file, "\ts%d -> s%d [label = <&#949;>]\n", i, L->state);
            else
                fprintf(file, "\ts%d -> s%d [label = %c]\n", i, L->state, L->symbol);
            L = L->next;
        }
    }
    fprintf(file, "}\n");
    fclose(file);
}

