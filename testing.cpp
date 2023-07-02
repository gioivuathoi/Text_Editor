#include<iostream>
#include "file_stack.h"
#include<string.h>
using namespace std;
int main(){
    stack S;
    init(&S);
    push(&S,"I love you");
    string data = pop(&S);
    int len = data.length();
    char *mydata = new char(len+1);
    strcpy(mydata,data.c_str());
    mydata[len] = '\0';
    string mydata1(mydata);
    cout<<mydata1;
    return 0;
}