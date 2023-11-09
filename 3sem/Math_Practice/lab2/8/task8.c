#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include "my_lib.h"


#define ull unsigned long long int
#define ll long long int

#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)

//Error handler

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

//if bad returns -1
int get_digit(char c)
{
    if(isdigit(c))
    {
        return c - '0';
    }
        
    if (isalpha(c))
    {
        return toupper(c) - 'A' + 10;
    }
    return -1;
}

status_code remove_zeros(char* input, char** output)
{
    if(input == NULL)
        return INVALID_PARAMETER;
    int len = strlen(input);
    int count = 0;

    for(int i = 0; i < len; i++)
    {
        if(input[i] == '0')
            count++;
        else
            break;
    }
    if(len == count)
    {
        *output = "0";
        return OK;
    }
    *output = (char*) calloc(len - count + 1, sizeof(char));
    if(output == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len - count; i++)
    {
        (*output)[i] = input[i + count];
    }
    (*output)[len - count] = '\0';
    return OK;
}

status_code reverse_str(char* str, char** result)
{
    int len = strlen(str);
    *result = (char*) malloc((len + 1) * sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len; i++)
    {
        (*result)[len - i - 1] = str[i];
    }
    (*result)[len] = '\0';
    return OK;
}


char get_char_of_digit(int a)
{
    if(a < 10)
        return (char)(a + '0');
    else
        return (char)('A' - 10 + a);
}

status_code sum(char* x, char* y, int base, char** result)
{
    if(x == NULL || y == NULL)
        return INVALID_PARAMETER;

    remove_zeros(x, &x);
    remove_zeros(y, &y);
    reverse_str(x, &x);
    reverse_str(y, &y);
    //00123 -> 321
    //00012 -> 21
    int lenx, leny;
    lenx = strlen(x);
    leny = strlen(y);
    int max_len = lenx > leny ? lenx + 1 : leny + 1;
    *result = (char*) calloc(max_len + 1, sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    int i = 0;
    int j = 0;
    int digit, digit2;
    
   
    int remember = 0;
    int counter = 0;
    while(i < lenx && j < leny)
    {
        digit = get_digit(x[i]);
        digit2 = get_digit(y[j]);
        if(digit == -1 || digit2 == -1 || digit >= base || digit2 >= base)
        {
            return INVALID_PARAMETER;
        }
        int res = (digit + digit2 + remember) % base;
        remember = (digit + digit2 + remember) / base;
        (*result)[counter] = get_char_of_digit(res);
        counter++;
        i++;
        j++;
    }
    while(i < lenx)
    {
        int d = get_digit(x[i]);
        if(d >= base || d == -1)
        {
            return INVALID_PARAMETER;
        }
        int res = (d + remember) % base;
        remember = (d + remember) / base;
        (*result)[counter] = get_char_of_digit(res);
        counter++;
        i++;
    }
    while(j < leny)
    {
        int d = get_digit(y[j]);
        if(d >= base || d == -1)
        {
            return INVALID_PARAMETER;
        }
        int res = (d + remember) % base;
        remember = (d + remember) / base;
        (*result)[counter] = get_char_of_digit(res);
        counter++;
        j++;
    }
    
    (*result)[counter] = get_char_of_digit(remember);
    (*result)[counter + 1] = '\0';
    reverse_str(*result, result);
    remove_zeros(*result, result);

    return OK;
}

status_code sum_all(int base, char** result, int count, ...)
{
    if(base < 2 || base > 36)
        return INVALID_PARAMETER;
    status_code code = OK;
    va_list argptr;
    va_start(argptr, count);

    char* res = (char*) va_arg(argptr, char*);
    if(count == 1)
    {
        *result = res;
        va_end(argptr);
        return OK;
    }
    char* a;
    for(int i = 0; i < count - 1; i++)
    {
        a = (char*) va_arg(argptr, char*);
        if ((code = sum(res, a, base, &res)) != OK)
        {
            return INVALID_PARAMETER;
        }
    }
    va_end(argptr);
    *result = res;
    return OK;
}

int main(int argc, char** argv)
{
    status_code code = OK;
    char* result;

    if((code = sum_all(16, &result, 3, "000000001", "00000001", "000000001")) != OK)
    {
        free(result);
        return show_error(code);
    }
    printf("Sum: %s", result);
    free(result);
    return OK;
}