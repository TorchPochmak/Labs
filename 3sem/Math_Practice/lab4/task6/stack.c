#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Определение стека
typedef struct node {
    char data;
    struct node* next;
} Node;


Stack* createStack()
{
    Stack* res = (Stack*) malloc(sizeof(Stack));
    res->top = NULL;
    return res;
}

// Функция для создания нового узла
Node* newNode(char data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

void push(Stack* st, char data)
{
    Node* new_node = newNode(data);
    new_node->next = st->top;
    st->top = new_node;
}

Node* pop(Stack* st)
{
    if(st->top == NULL)
        return NULL;
    Node* res = st->top;
    st->top = st->top->next;
    return res;
}

int prior(char c)
{
    if(c == '+' || c == '-')
        return 1;
    if(c == '*')
        return 2;
}

int main()
{
    char* input = "5+4*(3-4)";
    Queue* a = createQueue();
    Stack* st = createStack();

    for(int i = 0; i < strlen(input); i++)
    {
        if(isdigit(input[i]))
            enqueue(a, input[i]);
        else
        {
            if(input[i] == '(')
                push(st, '(');
            else if(input[i] == ')')
            {
                while(st->top->data != '(')
                {
                    Node* x = pop(st);
                    enqueue(a, x->data);
                    free(x);
                }
                pop(st);
            }
            else
            {
                int pr = prior(input[i]);
                if(st->top == NULL || st->top->data == '(' || prior(st->top->data) < pr)
                    push(st, input[i]);
                else
                {
                        while(st->top != NULL && (st->top->data != '(' || prior(st->top->data) >= pr))
                        {
                            Node* x = pop(st);
                            enqueue(a, x->data);
                            free(x);
                        }
                    push(st, input[i]);
                }
            }
        }
    }
    while(st->top != NULL)
    {
        enqueue(a, st->top->data);
        Node* x = pop(st);
        free(x);
    }
    char* res = (char*) calloc(strlen(input) + 1, sizeof(char));
    for(int i = 0; a->front != NULL; i++)
    {
        res[i] = a->front->data;
        dequeue(a);
    }
    res[strlen(input)] = '\0';
    printf("%s", res);
}