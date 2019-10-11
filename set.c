#include "set.h"

//list sort insert
void insertSet(set **S, int i)
{
    set *el, *cur = *S, *prev = NULL;
    while (cur && i > cur->info)
    {
        prev = cur;
        cur = cur->next;
    }
    if (cur && i == cur->info)
        return;
    el = malloc(sizeof(set));
    el->info = i;
    if (prev)
    {
        el->next = prev->next;
        prev->next = el;
    }
    else
    {
        el->next = *S;
        *S = el;
    }
}

void unionSet(set **A, set *B)
{
    while (B)
    {
        insertSet(A, B->info);
        B = B->next;
    }
}

int inSet(int i, set *S)
{
    while (S)
    {
        if (S->info == i)
            return 1;
        S = S->next;
    }
    return 0;
}

int lengthSet(set *S)
{
    int n = 0;
    while (S)
    {
        n++;
        S = S->next;
    }
    return n;
}

int equalSet(set *A, set *B)
{
    while (A && B)
    {
        if (A->info != B->info)
            return 0;
        A = A->next;
        B = B->next;
    }
    if (!A && !B)
        return 1;
    else
        return 0;
}

void printSet(FILE * f, set *S, char T)
{
    fprintf(f, "{");
    while (S)
    {
        if (T == 'i')
            fprintf(f, "%d", S->info);
        if (T == 'c')
            fprintf(f, "%c", S->info);
        S = S->next;
        if (S)
            fprintf(f,",");
    }
    fprintf(f,"}");
}

void disposeSet(set *S)
{
    while (S)
    {
        set *tmp = S;
        S = S->next;
        free(tmp);
    }
}
