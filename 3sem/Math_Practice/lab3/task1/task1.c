#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef unsigned long long int ull;
typedef long long int ll;
typedef unsigned char uchar;

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

int plus(int a, int b)
{
    int cr = 0;
    while(b)
    {
        cr = a & b;
        a = a ^ b;
        b = cr << 1;
    }
    return a;
}
int neg(int a)
{
    return plus(~a, 1);
}
int minus(int a, int b)
{
    return plus(a, neg(b));
}
int my_abs(int a)
{
    if(a < 0)
        return neg(a);
    return a;
}
int int_bits()
{
    int result = 0;
    for(int i = 0; i < sizeof(int);)
    {
        result = plus(result, 8);
        i = plus(i, 1);
    }
    return result;
}
int multiply(int a, int b) {
    if(a == 0 || b == 0)
        return 0;
    int has_minus = (a >> minus(int_bits(), 1)) ^ (b >> minus(int_bits(), 1));
    a = my_abs(a);
    b = my_abs(b);
    int result = 0;
    while (b != 0) {
        if (b & 1) {
            result = plus(result,a);
        }
        a <<= 1;
        b >>= 1;
    }
    if(has_minus)
        result = neg(result);
    return result;
}
//--------------------------------
//if bad returns -1
char get_char_of_digit(int a)
{
    if(a < 0)
        return '\0';
    if(a < 10)
        return (char)(plus(a,'0'));
    else
        return (char)(plus(minus('A',10),a));
}
status_code reverse_str(char* str, char** result)
{
    int len = strlen(str);
    *result = (char*) calloc((plus(len, 1)), sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len;)
    {
        (*result)[minus(minus(len,i),1)] = str[i];
        i = plus(i, 1);
    }
    (*result)[len] = '\0';
    return OK;
}

status_code convert_to_2rbase(int input, int r, char** output)
{
    if(r < 1 || r > 5)
        return INVALID_PARAMETER;
    if(input == INT_MIN)
        return MY_OVERFLOW;
    if(output == NULL)
        return ALLOC_ERROR;
    if(input == 0)
    {
        *output = "0";
        return OK;
    }
    bool has_minus = input >> minus(int_bits(), 1);
    int base = 1 << r;
    input = my_abs(input);

    int max_res_size = 1;
    int res_size = 0;
    char* result = (char*) calloc(2, sizeof(char));
    if(result == NULL)
        return ALLOC_ERROR;
    int bad_zeros = 0;
    for(int i = 0; multiply(i,r) < int_bits();)
    {
        input = input >> multiply(i,r);
        char digit = get_char_of_digit(input & minus(base, 1)); 
        result[res_size] = digit;
        res_size = plus(res_size, 1);
        if(digit == '0')
            bad_zeros++;
        else
            bad_zeros = 0;
        if(res_size == max_res_size)
        {
            max_res_size = multiply(max_res_size, 2);
            result = (char*) realloc(result, max_res_size);
            if(result == NULL)
                return ALLOC_ERROR;
        }
        i = plus(i, 1);
    }
    res_size = minus(res_size, bad_zeros);
    if(minus(max_res_size,res_size) <= 2)
    {
        max_res_size = plus(max_res_size, 2);
        result = (char*) realloc(result, max_res_size);
        if(result == NULL)
            return ALLOC_ERROR;
    }
    if(has_minus)
    {
        result[res_size] = '-';
        res_size = plus(res_size, 1);
    }
    result[res_size] = '\0';
    if(reverse_str(result, &result) != OK)
    {
        free(result);
        return OK;
    }
    *output = result;
    return OK;

}
int main()
{
    char* a;
    convert_to_2rbase(10,3, &a);
    printf("%s\n", a);
    //
    convert_to_2rbase(-16,4, &a);
    printf("%s\n", a);
    convert_to_2rbase(-255,4, &a);
    printf("%s\n", a);
    convert_to_2rbase(255,1, &a);
    printf("%s\n", a);
    convert_to_2rbase(10,2, &a);
    printf("%s\n", a);
    convert_to_2rbase(-10,3, &a);
    printf("%s\n", a);
    convert_to_2rbase(0,4, &a);
    printf("%s\n", a);
    convert_to_2rbase(1,2, &a);
    printf("%s\n", a);
}