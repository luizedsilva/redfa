/*--------------------------------------------------
 * Converte Expressao Regular em notação NPR
 * Usa + = uniao, . = concatenacao e * = fecho kleene
 *---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 10
#define TAM 100

typedef struct no *ptno;
typedef struct no {
    char info;
    ptno next;
}no;

ptno push (ptno T, char c) {
    ptno n = (ptno) malloc (sizeof (no));
    n->info = c;
    n->next = T;
    return n; 
}

ptno pop (ptno T, char *c) {
    if (T) {
        ptno n = T;
        *c = T->info;
        T = T->next;
        free (n);
    }
    return T;
}

int prior(char c) {
    switch (c) {
        case '(': return 1;
        case '+': return 2;
        case '.': return 3;
        case '*': return 4;
    }
}

int isAlphabet (char c) {
   return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

void converte(char *infixa, char *npr) {
    ptno Top = NULL;
    int i, j;
    char c, aux;
    j = 0;
    for (i = 0; infixa[i]; i++) {
        c = tolower(infixa[i]);
        if (isAlphabet(c))
            npr[j++] = c;
        else
            if (c == '+' || c == '.' || c == '*') {
            while (Top && prior(c) <= prior(Top->info)) {
                Top = pop (Top, &aux);
                npr[j++] = aux;
            }
            Top = push(Top, c);
        } else
            if (c == '(')
            Top = push(Top, '(');
        else
            if (c == ')') {
            while (Top && Top->info != '(') {
                Top = pop (Top, &aux);
                npr[j++] = aux;
            }
            Top = pop(Top, &aux);
        }
    }
    while (Top) {
        Top = pop (Top, &aux);
        npr[j++] = aux;
    }
    npr[j] = '\0';
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

void adicionaPonto (char *ent, char *sai) {
    int i, j;
    sai[0] = ent[0];
    for (i = 1, j = 1; ent[i]; i++, j++) {
        if (ent[i] == '(') {
             if (ent[i-1] != '.' && (isAlphabet(ent[i-1]) || ent[i-1] == ')' || ent[i-1] == '*'))
                sai[j++] = '.';
            sai[j] = ent[i];
        }
        if (isAlphabet(ent[i])) {
            if (ent[i-1] != '.' && (isAlphabet(ent[i-1]) || ent[i-1] == ')' || ent[i-1] == '*'))
                sai[j++] = '.';
            sai[j] = ent[i];
        }
        else sai[j] = ent[i];
    }
    sai[j] = 0;
}

int main(int argc, char** argv) {

    char entrada[TAM], entdot[TAM];
    char saida[TAM];
    scanf ("%s", entrada);
    adicionaPonto (entrada, entdot);
    puts (entdot);
    converte(entdot, saida);
    printf("Expressao Infixa de Entrada = %s, NPR = %s\n\n", entrada, saida);
    // geraAFN(saida);
    return (EXIT_SUCCESS);
}

