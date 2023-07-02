#include "file_stack.h"

void init(stack *S)
{
    S->top = NULL;
}
bool isempty(stack S)
{
    return (S.top == NULL);
}
int length(stack *S)
{
    node *P = S->top;
    int i = 0;
    while (P != NULL)
    {
        i++;
        P = P->next;
    }
    return i;
}

node *makeanode(string ncontent)
{
    node *P = new node;
    P->content = ncontent;
    P->next = NULL;
    return P;
}
void push(stack *S, string ncontent) {
    if(length(S) == 10) string a = leak(S);
    if(length(S) >=1 && ncontent == S->top->content) string a = pop(S);
    node *P = makeanode(ncontent);
    P->next = S->top;
    S->top = P;
}
string peak(stack S)
{
    return S.top->content;
}
string pop(stack *S)
{
    if (!isempty(*S))
    {
        string x = S->top->content;
        node* a = S->top;
        S->top = S->top->next;
        delete a;
        return x;
    }
}
void clear(stack *S)
{
    while(!isempty(*S))
    {
        node* a = S->top;
        S->top = S->top->next;
        delete a;
    }
}
string leak(stack *S)
{
    node *a = S->top;
    while (a->next->next != NULL)
    {
        a = a->next;
    }
    string x = a->next->content;
    a->next = NULL;
    return x;
}
