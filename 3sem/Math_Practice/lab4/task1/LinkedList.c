#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include <String.h>

//Linked list of Node_ptr

typedef unsigned long long ull;

typedef enum 
{
    OK,
    INVALID_PARAMETER,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    ALLOC_ERROR,
    FILE_ERROR,
    INPUT_ERROR,
    UNKNOWN_ERROR,
    RESERVED
} status_code;

typedef struct Node
{
    char* key;
    char* value;
    ull maxhash_value;
    ull cur_hashvalue;
    struct Node* prev;
    struct Node* next;
} Node, *Node_ptr;

typedef struct LinkedList
{
    int size;
    Node* head;
    Node* tail;
} LinkedList, *LinkedList_ptr;

status_code list_create(LinkedList_ptr* list)
{
    *list = (LinkedList_ptr) calloc(1, sizeof(LinkedList));
    if(!list)
        return ALLOC_ERROR;
    return OK;
}

status_code list_push_back(LinkedList_ptr* list_res, Node_ptr new_node)
{
    LinkedList_ptr list = *list_res;
    status_code error = OK;
    if(list == NULL || new_node == NULL)
        return INVALID_PARAMETER;
    if(list->size == 0)
    {
        list->head = new_node;
        list->tail = new_node;
        return OK;
    }
    list->tail->next = new_node;
    list->tail->next->prev = list->tail;
    list->tail->next->next = NULL;
    list->size += 1;
    list->tail = list->tail->next;
    return OK;
}

void free_list(LinkedList_ptr list)
{
    if(!list)
        return;
    Node_ptr cur = list->head;
    Node_ptr to_free = NULL;
    while(cur->next != NULL)
    {
        to_free = cur;
        cur = cur->next;
        free_node(to_free);
        to_free = NULL;
    }   
    free_node(cur);
    cur = NULL;
    free(list);
    list = NULL;
}