#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include <String.h>

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

const char* error_details[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Input error\n",
    "ERROR: Unknown...\n",
    "ERROR: Reserved...\n"
};

int show_error(status_code code)
{
    printf("%s", error_details[code]);
    return code;
}

typedef struct Node
{
    char* key;
    char* value;
    ull maxhash_value;
    ull cur_hashvalue;
    struct Node* prev;
    struct Node* next;
} Node, *Node_ptr;

typedef struct NodeList
{
    int size;
    Node* head;
    Node* tail;
} NodeList, *NodeList_ptr;

const ull MAX_HASH = 2147483648;
typedef struct HashTable
{
    NodeList* lists;//array of lists
    int lists_cnt;
    ull hash_size;//< MAX_HASH, count of NodeLists
} HashTable, *HashTable_ptr;

void free_node(Node_ptr node);
void free_list(NodeList_ptr list);
void free_table(HashTable_ptr table);


int get_digit(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if(isalpha(c) && isupper(c))
        return c - 'A' + 10;
    if(isalpha(c) && islower(c))
        return c - 'a' + 10;
    return -1;
}

ull hash_function(char* str, ull mod)
{
    if(!str)
        return 0;
    ull result = 1;
    if(str == NULL)
        return 0;
    for(int i = 0; i < strlen(str); i++)
    {
        int digit = get_digit(str[i]);
        if(digit < 0)
            return 0;
        if(i != 0)
            result *= 62;
        result %= mod;
    }
    return OK;
}
//returns a pointer to a hashtable

//return a pointer to created node
status_code create_node(char* key, char* value, Node_ptr* resnode)
{
    Node_ptr node = *resnode;
    node = (Node_ptr) calloc(1, sizeof(Node));
    if(!node)
        return ALLOC_ERROR;
    node->key = key;
    node->value = value;
    node->maxhash_value = hash_function(key, MAX_HASH);
    return OK;
}

//create_list == calloc(1, sizeof(NodeList));

status_code push_back(NodeList_ptr* reslist, Node_ptr new_node)
{
    NodeList_ptr list = *reslist;
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

status_code find_node(NodeList_ptr list, char* key, Node_ptr* result)
{
    if(list == NULL || key == NULL)
        return INVALID_PARAMETER;

    Node_ptr cur_node = list->head;
    for(int i = 0; i < list->size; i++)
    {
        if(!strcmp(cur_node->key, key))
        {
            *result = cur_node;
            return OK;
        }
        cur_node = cur_node->next;
    }
    *result = NULL;
    return OK;
}

status_code create_hash_table(HashTable_ptr* res, ull hash_size)
{
    HashTable_ptr result;
    result = (HashTable_ptr) calloc(1, sizeof(HashTable));
    if(!result)
        return ALLOC_ERROR;
    result->hash_size = hash_size;
    result->lists = (NodeList_ptr) calloc(hash_size, sizeof(NodeList));
    if(!(result->lists))
    {
        free_table(result);
        return ALLOC_ERROR;
    }
    result->lists_cnt = hash_size;
    *res = result;
    return OK;
}

status_code insert_element(HashTable_ptr* table, Node_ptr element)
{
    if(table == NULL || (*table) == NULL || (*table)->lists == NULL || element == NULL)
        return INVALID_PARAMETER;
    HashTable_ptr cur_table = *table;
    status_code error = OK;
    NodeList_ptr ptr = &(cur_table->lists[element->cur_hashvalue]);
    error = push_back(&ptr, element);
    return error;
}

status_code rehash(HashTable_ptr* table, NodeList_ptr cache, ull new_hash)
{
    if(cache == NULL || cache->head == NULL || cache->tail == NULL || table == NULL)
        return INVALID_PARAMETER;
    status_code error = OK;
    HashTable_ptr result;

    error = create_hash_table(&result, new_hash);
    if(error) return error;

    Node* node = cache->head;
    while(node->next != NULL)
    {
        node->cur_hashvalue = node->maxhash_value % new_hash;//new hash!!!
        error = insert_element(&result, node);
        if(error)
        {
            free_table(*table);
            return error;
        }
        node = node->next;
    }
    return OK;
}

status_code increase_hash_size(HashTable* table)
{
    table->hash_size *= 2;
    if(table->hash_size > MAX_HASH)
    {
        free_table(table);
        return MY_OVERFLOW;
    }
    //TODO d
    return OK;
}

void free_node(Node_ptr node)
{
    if(!node)
        return;
    free(node->key);
    node->key = NULL;
    free(node->value);
    node->value = NULL;
    free(node);
    node = NULL;
}

void free_list(NodeList_ptr list)
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

void free_table(HashTable_ptr table)
{
    if(!table)
        return;
    for(int i = 0; i < table->lists_cnt; i++)
    {
        NodeList_ptr ptr = &(table->lists[i]);
        free_list(ptr);
        ptr = NULL;
    }
    table->lists = NULL;
    free(table);
    table = NULL;
}

void print_table(HashTable_ptr table)
{
    for(int i = 0; i < table->hash_size; i++)
    {
        printf("key:%d\n", i);
        NodeList_ptr list = &(table->lists[i]);
        Node_ptr cur_node = list->head;
        for(int i = 0; i < list->size; i++)
        {
            printf("%s ", cur_node->value);
            cur_node = cur_node->next;
        }
    }
}

int main()
{
    
}