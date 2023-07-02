#ifndef FILE_STACK
#define FILE_STACK
#include <string>
using namespace std;

typedef struct node
{
    string content;
    node *next;
}node;

typedef struct stack
{
    node *top;
}stack;
void init(stack*); 
bool isempty(stack); 
int length(stack*);  
node *makeanode();   
void push(stack*, string);        
string pop(stack*);  
void clear(stack*);
string leak(stack*);
#endif