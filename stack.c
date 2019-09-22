//Stack Pointer Structure
//-----------------------
typedef struct no *stack;
typedef struct no
{
    void *info;
    stack next;
} no;

void push(stack *T, void *A)
{
    stack n = (stack)malloc(sizeof(no));
    n->info = A;
    n->next = *T;
    *T = n;
}

void *pop(stack *T)
{
    if (*T)
    {
        stack n = *T;
        void *A = (*T)->info;
        *T = (*T)->next;
        free(n);
        return A;
    }
    puts("Stack underflow!");
    exit(1);
}
