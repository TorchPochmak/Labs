#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct 
{
    int size;
    char* data;
} String;

//NULL if failed
String* string_create(const char* data);
//NULL if failed
String* string_create_from(const String* str)
void string_destroy(String* ptr);
//NULL if failed
String* string_concat(const String* str1, const String* str2);
//First greater if failed
int string_compare(const void* str1v, const void* str2v);
bool string_equal(const String* str1, const String* str2);
//1 if failed
int string_copy(String* from, String* to);

