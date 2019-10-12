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

#include "stack.h"

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
