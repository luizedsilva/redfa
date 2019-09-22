//Set structure
//-------------
typedef struct set
{
    int info;
    struct set *next;
} set;

void insertSet(set **S, int i)
{
    set *el, *cur = *S, *prev = NULL;
    while (cur)
    {
        prev = cur;
        if (cur->info == i)
            return;
        cur = cur->next;
    }
    el = malloc(sizeof(set));
    el->info = i;
    el->next = NULL;
    if (prev)
        prev->next = el;
    else
        *S = el;
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
    if (!A && !B)
        return 1;
    if (!A && B)
        return 0;
    if (A && !B)
        return 0;
    while (A)
    {
        int found = 0;
        while (B && !found)
        {
            if (A->info == B->info)
                found = 1;
            B = B->next;
        }
        if (!found)
            return 0;
        A = A->next;
    }
    return 1;
}

void printSet(set *S, char T)
{
    printf("{");
    while (S)
    {
        if (T == 'i')
            printf("%d", S->info);
        if (T == 'c')
            printf("%c", S->info);
        S = S->next;
        if (S)
            printf(",");
    }
    printf("}");
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
