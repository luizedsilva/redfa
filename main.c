/*-----------------------------------------------------------------------
 * redfa - Convert Regex to Minimized Determinitic Finite Automata
 * Using | to union, . (not digited) to concat and * to kleene closure
 * By Luiz Eduardo da Silva - 2019
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>
 *-----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "structures.h"
#include "set.h"
#include "stack.h"
#include "dfa.h"
#include "nfa.h"

// functions to convert regex to regex in npr
int prior(char c)
{
    switch (c)
    {
    case '(':
        return 1;
    case '|':
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
        else if (c == '|' || c == '.' || c == '*')
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
    char *input, *inputDot, *inputNPR;
    nfa *N;
    dfa *D, *Dmin;
    int display = 0, generate = 0, show = 0;
    if (argc < 2)
    {
        printf("Translate Regular Expression on Deterministic Finite Automata\n");
        printf("\nUsage:%s <RegEx> [Options]\n", argv[0]);
        printf("\nwhere:");
        printf("\tRegex = Number or Letter or '|' or '*'\n");
        printf("\nOptions:\n");
        printf("\t-d display dfa/nfa strutures\n");
        printf("\t-g generates graph .dot files\n");
        printf("\t-s show set of corresponding dfa/nfa states\n");                
        printf("\nExample: %s \"(1|0)*\" -d\n", argv[0]);
        input = malloc(10 * sizeof(char));
        strcpy(input, "(1|0)*");
        display = 1;
    }
    else
    {
        int i;
        for (i = 2; i < argc; i++) {
            if (argv[i][0] == '-') {
                switch (argv[i][1]) {
                    case 'd': display = 1; break;
                    case 'g': generate = 1; break;
                    case 's': show = 1; break;
                }
            }
        }
        input = malloc(sizeof(argv[1]) * sizeof(char) + 1);
        strcpy(input, argv[1]);
    }

    // Regex convertion
    inputDot = malloc(2 * strlen(input) * sizeof(char));
    inputNPR = malloc(2 * strlen(input) * sizeof(char));
    addDot(input, inputDot);
    convert(inputDot, inputNPR);

    // NFA and DFA convertions
    N = regexToNfa(inputNPR);
    D = nfaToDfa(N);
    Dmin = minimize(D);

    if (display) {
       displayNfaAutomata(N, input);    
       displayDfaAutomata(D, input);
       displayDfaAutomata(Dmin, input);
    }

    if (generate) {
       // NFA dot and png files generation
       saveNfaDotFile(N, "afn.dot", input);
       system("dot -Tpng afn.dot -o afn.png");
       system("eog afn.png&");

       // DFA dot and png files generation
       saveDfaDotFile(D, "afd.dot", input, show);
       system("dot -Tpng afd.dot -o afd.png");
       system("eog afd.png&");

       //DFA minimal dot and png files generation
       saveDfaDotFile(Dmin, "afdmin.dot", input, show);
       system("dot -Tpng afdmin.dot -o afdmin.png");
       system("eog afdmin.png&");
    }

    disposeNfaAutomata(N);
    disposeDfaAutomata(D);
    disposeDfaAutomata(Dmin);
    free(inputDot);
    free(inputNPR);

    return 0;
}
