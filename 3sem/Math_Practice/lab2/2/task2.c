#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include "my_lib.h"

//++++++++

#define ull unsigned long long int
#define ll long long int

#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)

//Fuck recursion all my homies use while()
status_code bin_pow_no_rec(ll a, ll n, ll* res) {
    if ((n == 0 && a == 0) || (n < 0) || (a == LLONG_MIN))
    {
        return INVALID_PARAMETER;
    }

    ll multiplier = 1;
    bool overflow = false;

    //Работа с модулем
    *res = 1;
    if (a < 0 && (n & 1 == 1))
    {
        multiplier = -1;
    }
    a = abs(a);

    while (n) 
    {
        if (n & 1)
        {
            overflow =  overflow || mult(*res, a, res);
        }
        overflow =  overflow || mult(a,a,&a);
        n >>= 1;
    }

    if (!overflow)
    {
        *res = *res * multiplier;
        return OK;
    }
    else
    {
        return MY_OVERFLOW;
    }
}

status_code bin_pow_rec(ll a, ll n, ll* res)
{
    status_code code = OK;
    if ((n == 0 && a == 0) || (n < 0) || (a == LLONG_MIN))
    {
        return INVALID_PARAMETER;
    }
    bool overflow = false;
    if(n == 0)
    {
        *res = 1;
        return OK;
    }
    else if(n % 2 == 0)
    {
        ll r1;
        if((code = bin_pow_rec(a, n / 2, &r1)) != OK)
            return code;
        overflow = overflow || mult(r1, r1, res);
    }
    else if(n % 2 == 1)
    {
        ll r1;
        if((code = bin_pow_rec(a, n - 1, &r1)) != OK)
            return code;
        overflow = overflow || mult(r1, a, res);
    }
    if(overflow)
        return MY_OVERFLOW;
    else
        return OK;

}

status_code geometric_aver(double* result, int count, ...)
{
    if(count <= 0)
        return INVALID_PARAMETER;
    va_list argptr;
    va_start(argptr, count);
    *result = 1.0;
    for(int i = 0; i < count; i++)
    {
        *result *= va_arg(argptr, double);
    }
    va_end(argptr);
    *result = pow(*result, 1.0/((double) count));
    return OK;
}

int main(int argc, char** argv)
{
    status_code code = OK;
    ll test_res;
    ll test_a = 5;
    ll test_n = 3;
    if((code = bin_pow_rec(test_a, test_n, &test_res)) != OK)
        return show_error(code);
    printf("%ld^%ld=%ld\n", test_a, test_n, test_res);

    test_a = 40;
    test_n = 5;
    if((code = bin_pow_rec(test_a, test_n, &test_res)) != OK)
        return show_error(code);
    printf("%ld^%ld=%ld\n", test_a, test_n, test_res);

    // test_a = 0;
    // test_n = 0;
    // if((code = bin_pow_rec(test_a, test_n, &test_res)) != OK)
    //     return show_error(code);
    // printf("%ld^%ld=%ld\n", test_a, test_n, test_res);

    double g_test;
    if((code = geometric_aver(&g_test, 5, 1.0, 2.0, 3.0, 4.0, 5.0)) != OK)
        return show_error(code);
    
    printf("%lf compared to %lf", g_test, pow(1.0 * 2.0 * 3.0 * 4.0 * 5.0, 1.0/5.0));
    return 0;
}