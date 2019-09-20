/*--------------------------------------------------
 * Convert Regex to Nondeterminitic Finite Automata
 * Use + to union, . to concat and * to kleene closure
 *---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TAM 100
#define EPSILON ' '

//Automata Structure
//-------------------------
//  [ ] nStates
//  transitions(array):
//         0 [ ]-> [symbol|state|next] -> [.|.|/]
//         1 [ ]-> ...
//  ...
//  nStates-1[ ]-> ...
//  0 = initial state, nStates-1 = final state

typedef struct link
{
    char symbol;
    int state;
    struct link *next;
} link;

typedef struct
{
    int nStates;
    struct link **transitions;
} automata;

void insertLink(struct link **L, char symbol, int state)
{
    struct link *n = malloc(sizeof(struct link));
    n->state = state;
    n->symbol = symbol;
    n->next = *L;
    *L = n;
}

//Stack Automata Structure
//--------------------------
typedef struct no *ptno;
typedef struct no
{
    automata *info;
    ptno next;
} no;

void push(ptno *T, automata *A)
{
    ptno n = (ptno)malloc(sizeof(no));
    n->info = A;
    n->next = *T;
    *T = n;
}

automata *pop(ptno *T)
{
    if (*T)
    {
        ptno n = *T;
        automata *A = (*T)->info;
        *T = (*T)->next;
        free(n);
        return A;
    }
    return NULL;
}

//Automata Functions
//---------------------
automata *buildBasic(char symbol)
{
    automata *A = malloc(sizeof(automata));
    struct link *L = malloc(sizeof(struct link));
    A->nStates = 2;
    A->transitions = malloc(2 * sizeof(struct link *));
    A->transitions[0] = NULL;
    A->transitions[1] = NULL;
    insertLink(A->transitions + 0, symbol, 1);
    return A;
}

automata *buildStar(automata *A)
{
    int i;
    automata *newA = malloc(sizeof(automata));
    newA->nStates = A->nStates + 2;
    newA->transitions = malloc(newA->nStates * sizeof(struct link *));
    for (i = 0; i < newA->nStates; i++)
    {
        newA->transitions[i] = NULL;
    }
    insertLink(newA->transitions + 0, EPSILON, 1);
    insertLink(newA->transitions + 0, EPSILON, newA->nStates - 1);
    for (i = 0; i < A->nStates; i++)
    {
        struct link *L = A->transitions[i];
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

automata *buildPlus(automata *A, automata *B)
{
    int i;
    automata *newA = malloc(sizeof(automata));
    newA->nStates = A->nStates + B->nStates + 2;
    newA->transitions = malloc(newA->nStates * sizeof(struct link *));
    for (i = 0; i < newA->nStates; i++)
    {
        newA->transitions[i] = NULL;
    }
    insertLink(newA->transitions + 0, EPSILON, 1);
    insertLink(newA->transitions + 0, EPSILON, A->nStates + 1);
    for (i = 0; i < A->nStates; i++)
    {
        struct link *L = A->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + i + 1, L->symbol, L->state + 1);
            L = L->next;
        }
    }
    for (i = 0; i < B->nStates; i++)
    {
        struct link *L = B->transitions[i];
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

automata *buildDot(automata *A, automata *B)
{
    int i;
    automata *newA = malloc(sizeof(automata));
    newA->nStates = A->nStates + B->nStates + 2;
    newA->transitions = malloc(newA->nStates * sizeof(struct link *));
    for (i = 0; i < newA->nStates; i++)
    {
        newA->transitions[i] = NULL;
    }
    insertLink(newA->transitions + 0, EPSILON, 1);
    for (i = 0; i < A->nStates; i++)
    {
        struct link *L = A->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + i + 1, L->symbol, L->state + 1);
            L = L->next;
        }
    }
    insertLink(newA->transitions + A->nStates, EPSILON, A->nStates + 1);    
    for (i = 0; i < B->nStates; i++)
    {
        struct link *L = B->transitions[i];
        while (L)
        {
            insertLink(newA->transitions + A->nStates + i + 1, L->symbol, L->state + A->nStates + 1);
            L = L->next;
        }
    }
    insertLink(newA->transitions + newA->nStates - 2, EPSILON, newA->nStates - 1);
    return newA;
}


void disposeAutomata(automata *A)
{
    int i;
    for (i = 0; i < A->nStates; i++)
    {
        struct link *L = A->transitions[i];
        while (L)
        {
            struct link *temp = L;
            L = L->next;
            free(temp);
        }
    }
    free(A);
}

void displayAutomata(automata *A)
{
    int i;
    printf("nStates = %d\n", A->nStates);
    printf("Transitions:\n");
    for (i = 0; i < A->nStates; i++)
    {
        struct link *L = A->transitions[i];
        printf("\tState %d : ", i);
        while (L)
        {
            printf("[%c|%d] ", L->symbol, L->state);
            L = L->next;
        }
        printf("\n");
    }
}

int prior(char c)
{
    switch (c)
    {
    case '(':
        return 1;
    case '+':
        return 2;
    case '.':
        return 3;
    case '*':
        return 4;
    }
}

int isAlphabet(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

void convert(char *infix, char *npr)
{
    char c, *stack = (char *)malloc(sizeof(char) * strlen(infix) + 1);
    int i, j, top = -1;
    for (j = 0, i = 0; infix[i]; i++)
    {
        c = tolower(infix[i]);
        if (isAlphabet(c))
            npr[j++] = c;
        else if (c == '+' || c == '.' || c == '*')
        {
            while (top >= 0 && prior(c) <= prior(stack[top]))
                npr[j++] = stack[top--];
            stack[++top] = c;
        }
        else if (c == '(')
            stack[++top] = c;
        else if (c == ')')
        {
            while (top >= 0 && stack[top] != '(')
                npr[j++] = stack[top--];
            top--;
        }
    }
    while (top >= 0)
        npr[j++] = stack[top--];
    npr[j] = '\0';
}

void addDot(char *in, char *out)
{
    int i, j;
    out[0] = in[0];
    for (i = 1, j = 1; in[i]; i++, j++)
    {
        if (in[i] == '(')
        {
            if (in[i - 1] != '.' && (isAlphabet(in[i - 1]) || in[i - 1] == ')' || in[i - 1] == '*'))
                out[j++] = '.';
            out[j] = in[i];
        }
        if (isAlphabet(in[i]))
        {
            if (in[i - 1] != '.' && (isAlphabet(in[i - 1]) || in[i - 1] == ')' || in[i - 1] == '*'))
                out[j++] = '.';
            out[j] = in[i];
        }
        else
            out[j] = in[i];
    }
    out[j] = 0;
}

// void geraAFN (char *er) {
//     pilha P;
//     int i;
//     char c;
//     char afn1[TAM], afn2[TAM], str[TAM];
//     iniciaPilha (&P);
//     for (i = 0; er[i]; i++) {
//         c = tolower(er[i]);
//         if (isAlphabet(c)) {
//             printf ("Gera AFN %c\n", c);
//             sprintf (afn1, "af(%c)", c);
//             empilha (&P, afn1);
//         }
//         if (c == '*') {
//             desempilha(&P, afn1);
//             printf ("Gera AFN estrela %s\n", afn1);
//             sprintf (str, "af*(%s)", afn1);
//             empilha (&P, str);
//         }
//         if (c == '.') {
//             desempilha(&P, afn2);
//             desempilha(&P, afn1);
//             printf ("Gera AFN ponto %s %s\n", afn1, afn2);
//             sprintf (str, "af.(%s, %s)", afn1, afn2);
//             empilha (&P, str);
//         }
//         if (c == '+') {
//             desempilha(&P, afn2);
//             desempilha(&P, afn1);
//             printf ("Gera AFN mais %s %s\n", afn1, afn2);
//             sprintf (str, "af+(%s, %s)", afn1, afn2);
//             empilha (&P, str);
//         }
//     }
//     desempilha(&P, afn1);
//     printf ("RESULTADO %s\n", afn1);
// }

int main(int argc, char **argv)
{

    char input[TAM], inputDot[TAM], output[TAM];

    automata *A = buildBasic('a');
    displayAutomata(A);
    automata *B = buildBasic('b');
    displayAutomata(B);
    automata *Dot = buildDot (A, B);
    displayAutomata (Dot);
    automata *Union = buildPlus (A, B);
    displayAutomata(Union);
    automata *AStar = buildStar(Union);
    displayAutomata(AStar);
    automata *dotB = buildDot (AStar, B);
    displayAutomata(dotB);

    return 0;

    scanf("%s", input);
    addDot(input, inputDot);
    puts(inputDot);
    convert(inputDot, output);
    printf("Expressao Infixa de input = %s, NPR = %s\n\n", input, output);
    // geraAFN(output);
    return (EXIT_SUCCESS);
}
