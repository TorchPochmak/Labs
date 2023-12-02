#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include <String.h>

typedef struct 
{
    int size;
    char* data;
} String;

//NULL if failed or NULL
String* string_create(const char* data)
{
    if(data == NULL)
        return NULL;
    String* res = (String*) malloc(sizeof(String));
    res->size = strlen(data);
    res->data = (char*) malloc(sizeof(char) * (res->size + 1));
    if(!res->data)
    {
        free(res);
        return NULL;
    }
    strcpy(res->data, data);
    return res;
}
//NULL if failed
String* string_create_from(const String* str)
{
    if(str == NULL)
        return NULL;
    String* res = string_create(str->data);
    return res;
}
void string_destroy(String* ptr)
{
    if(ptr == NULL)
        return;
    free(ptr->data); ptr->data = NULL;
    free(ptr); ptr = NULL;
}
//NULL if failed
String* string_concat(const String* str1, const String* str2)
{
    if(str1 == NULL || str2 == NULL)
        return NULL;
    String* res = (String*) malloc(sizeof(String));
    if(res == NULL)
        return NULL;
    int n1 = str1->size;
    int n2 = str2->size;
    res->size = n1 + n2 + 1;
    res->data = (char*) malloc(sizeof(char) * (n1 + n2 + 1));
    if(res->data == NULL)
    {
        string_destroy(res);
        return NULL;
    }
    for(int i = 0; i < n1; i++)
    {
        res->data[i] = str1->data[i];
    }
    for(int i = 0; i < n2; i++)
    {
        res->data[i + n1] = str2->data[i];
    }
    res->data[n1 + n2] = '\0';
    return res;
}
//First greater if failed
int string_compare(const void* str1v, const void* str2v)
{
    String* str1 = (String*) str1v;
    String* str2 = (String*) str2v;
    if(str1 == NULL && str2 == NULL)
        return 0;
    if(str1 == NULL || str2 == NULL || str1->data == NULL || str2->data == NULL)
        return 1;
    int res = str1->size == str2->size ? 0 : 
        (str1->size - str2->size > 0 ? 1 : -1);
    if(res == 0)
    {
        int i = 0;
        while(str1->data[i] == str2->data[i] && i != str1->size)
            i++;
        if(i == str1->size)
            return 0;
        return str1->data[i] - str2->data[i] > 0 ? 1 : -1;
    }
    return res;
}
bool string_equal(const String* str1, const String* str2)
{
    return string_compare((void*)str1, (void*)str2) == 0 ? true : false;
}
//1 if failed
void string_copy(String* from, String* to)
{
    if(from == NULL)
    {
        to = NULL;
        return; 
    }
    to->size = from->size;
    strcpy(from->data, to->data);
}
int main()
{
    
}