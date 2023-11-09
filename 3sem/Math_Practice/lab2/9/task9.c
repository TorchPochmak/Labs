#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
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


double EPS = 1;
//use in main
void calc_eps()
{
    double prev;
    while ((1+EPS) != 1) 
    { 
        prev = EPS;
        EPS /=2; 
    } 
}

int gcd(int a, int b) 
{ 
    while (a > 0 && b > 0) { 
        if (a > b) { 
            a = a % b; 
        } 
        else { 
            b = b % a; 
        } 
    } 
    if (a == 0) { 
        return b; 
    } 
    return a; 
} 



status_code solve_for_one(int base, int* result, double num) {
    if (base < 2) {
        return INVALID_PARAMETER;
    }

    //найдем представление в виде числителя и знаменателя
    int up, down = 1;
    while (abs(num - floor(num)) > EPS) {
        num *= 10;
        down *= 10;
        if(abs(down) > INT_MAX / 10 || abs(num) > INT_MAX / 10)
            return MY_OVERFLOW;
    }
    //Сокращаем дробь
    up = num;
    int g = gcd(up, down);
    if(g != 1)
    {
        up /= g;
        down /= g;
    }
    //условие - down если основание не делится на простой делитель знаменателя, то все плохо 
    //поиск до корня
    //проверка на 2
    while(down % 2 == 0)
    {
        if(base % 2 == 0)
        {
            *result = false;
            return OK;
        }
        down /= 2;
    }
    for(int i = 3; i <= down; i += 2)
    {
        if(down % i == 0 && base % i != 0)
        {
            *result = false;
            return OK;
        }
    }
    *result = true;
    return OK; 
}

status_code solve(int base, int** result, int count, ...)
{
    status_code code = OK;
    va_list argptr;
    va_start(argptr, count);
    *result = (int*) calloc(count, sizeof(int));
    if(result == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < count; i++)
    {
        int res;
        double in = va_arg(argptr, double);
        if((code = solve_for_one(base, &res, in)) != OK)
        {
            free(*result);
            return code;
        }
        (*result)[i] = res;
    }
    va_end(argptr);
    return OK;
}

int main() {
    //REQUIRED
    calc_eps();

    status_code code = OK;
    int* res;
    if( (code = solve(3, &res, 3, 0.1, 0.2, 0.3)) != OK)
    {
        free(res);
        return show_error(code);
    }
    print_int_arr(res, 3, " ");
    free(res);

    return OK;
}