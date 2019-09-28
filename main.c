/*--------------------------------------------------------
 * Convert Regex to Minimized Determinitic Finite Automata
 * Use + to union, . to concat and * to kleene closure
 * By Luiz Eduardo da Silva
 *--------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "structures.h"
#include "set.h"
#include "stack.h"
#include "dfa.h"
#include "nfa.h"

// functions to convertion regex to regex in npr
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
        else
        {
            printf("Symbol '%c' not allowed!\n", c);
            exit(1);
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

int main(int argc, char **argv)
{
    char *input, *inputDot, *output;
    nfa *N;
    dfa *D, *Dmin;
    if (argc < 2)
    {
        printf("Translate Regular Expression on Deterministic Finite Automata\n");
        printf("\nUse:%s <RegEx>\n\twhere Regex = Number|Letter|+|*\n", argv[0]);
        printf("\tExample: %s \"1(1+0)*0\"\n", argv[0]);
        input = malloc(10 * sizeof(char));
        strcpy(input, "1(0+1)*0");
    }
    else
    {
        input = malloc(sizeof(argv[1]) * sizeof(char) + 1);
        strcpy(input, argv[1]);
    }
    // Regex convertion
    inputDot = malloc(2 * strlen(input) * sizeof(char));
    output = malloc(2 * strlen(input) * sizeof(char));
    addDot(input, inputDot);
    convert(inputDot, output);

    // NFA and DFA convertions
    N = regexToNfa(output);
    displayNfaAutomata(N, output);
    D = nfaToDfa(N);
    displayDfaAutomata(D, output);
    Dmin = minimize(D);
    displayDfaAutomata(Dmin, output);

    // NFA dot and png files creation
    saveNfaDotFile(N, "afn.dot");
    system("dot -Tpng afn.dot -o afn.png");
    system("eog afn.png&");

    // DFA dot and png files creation
    saveDfaDotFile(D, "afd.dot");
    system("dot -Tpng afd.dot -o afd.png");
    system("eog afd.png&");

    // DFA minimal dot and png files creation
    saveDfaDotFile(Dmin, "afdmin.dot");
    system("dot -Tpng afdmin.dot -o afdmin.png");
    system("eog afdmin.png&");

    disposeNfaAutomata(N);
    disposeDfaAutomata(D);
    disposeDfaAutomata(Dmin);
    free(inputDot);
    free(output);

    return 0;
}
