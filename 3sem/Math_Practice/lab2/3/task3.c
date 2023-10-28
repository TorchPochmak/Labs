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


int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}

status_code solve_files(char* substr, char*** result, int count, ...)
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

status_code solve_file(char* substr, char** result, FILE* file)
{
//заебался
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