#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include "my_lib.h"

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
    RESERVED,
} status_code;

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Input error\n"
    "ERROR: Unknown...\n",
    "ERROR: Specific...\n"
};

typedef struct
{
    int line;
    int index;
} substr_found;


int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}

status_code solve_files(char* substr, int** result_cnt, substr_found*** subs, int count, ...)
{
    status_code code = OK;
    va_list argptr;
    va_start(argptr, count);

    char* result_file_strings = NULL;

    for(int i = 0; i < count; i++)
    {
        FILE* f = va_arg(argptr, FILE*);
        if((code = solve_file(substr, &result_file_strings, f)) != OK)
        {
            free(result);
            free(result_file_strings);
        }

    }
    va_end(argptr);
    free(result);
    free(result_file_strings);
    return OK;
}

int mystrlen(char* str)
{
    if(str == NULL)
        return 0;
    int i = 0;
    while(str[i] != '\0')
        i++;
    return i;
}

status_code solve_file(char* substr, int* count, substr_found** result, FILE* file)
{
    if(file == NULL || substr == NULL)
        return INVALID_PARAMETER;
    int len = mystrlen(substr);
    char* buffer = (char*) malloc(sizeof(char) * (len + 1));
    if(buffer == NULL)
        return ALLOC_ERROR;
    buffer[len] = '\0';

    int result_size = 0;
    int max_size = 1;
    *result = (substr_found*) malloc(sizeof(substr_found) * max_size);
    if(*result == NULL)
    {
        free(buffer);
        return ALLOC_ERROR;
    }
    char c;
    int res
    while ((c = getc(file)) != EOF)
	{

    }
}

int main(int argc, char** argv)
{
    status_code code = OK;
    if(argv < 3)
        return show_error(INPUT_ERROR);

    for(int i = 0; i < argc - 2; i++)
    {
        FILE* file = 
    }
    
    return OK;
}