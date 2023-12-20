include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>
//#include "my_lib.h"

void print_int_arr(int* arr, int count, char* sep)
{
    if(sep == NULL)
        sep = " ";
    for(int i = 0; i < count - 1; i++)
    {
        printf("%d%s", arr[i], sep);
    }
    printf("%d", arr[count - 1]);
}
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

int main(int argc, char** argv)
{


    
}