#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include "my_lib.h"


#define ull unsigned long long int
#define ll long long int

#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)


//[0.5; 4] -> x = 1.04719...
double coss(double x)
{
    return cos(x) - 0.5;
}
//[-10;10] -> x = 1
double f1(double x)
{
    return exp(x) - exp(1);
}
//[-10;10] -> x = 2
double f2(double x)
{
    return 2 * x * x * x  - 3 * x * x - 4;
}
//[a,b]
status_code dichotomy(double a, double b, double eps, double (*f)(double), bool* is_success, double* result)
{
    if(eps <= 0 || equal_d(b - a, 0, eps) || b - a < 0)
        return INVALID_PARAMETER;
    double a_res = f(a);
    double b_res = f(b);
    if(a_res * b_res > eps)
    {
        *is_success = false;
        return OK;
    }
    double mid;
    while(fabs(b - a) > eps)
    {
        mid = (a + b) / 2.0;
        if(f(mid) * a_res > eps)
            a = mid;
        else
            b = mid;
    }
    *result = (a + b) / 2.0;
    *is_success = true;
    return OK;
}


int main(int argc, char** argv)
{
    status_code code = OK;
    bool is_success = true;
    double result = 0;


    if((code = dichotomy(0.5,4,1e-7, coss, &is_success, &result)) != OK)
        return show_error(code);
    if(is_success)
        printf("cos(x) - 0.5 = 0, x in [0.5;4], x = %lf", result);
    else
        printf("cos(x) - 0.5 = 0, x in [0.5;4], failed");
    printf("\n");
    //----------------------------------------------------------------
    if((code = dichotomy(-10.0,10.0,1e-7, f1, &is_success, &result)) != OK)
        return show_error(code);
    if(is_success)
        printf("exp(x) - exp(1) = 0, x in [-10;10], x = %lf", result);
    else
        printf("exp(x) - exp(1) = 0, x in [-10;10], failed");
    printf("\n");
    //----------------------------------------------------------------
    if((code = dichotomy(-10.0,10.0,1e-7, f2, &is_success, &result)) != OK)
        return show_error(code);
    if(is_success)
        printf("2x^3 - 3x^2 - 4 = 0, x in [-10;10], x = %lf", result);
    else
        printf("2x^3 - 3x^2 - 4 = 0, x in [-10;10], failed");
    printf("\n");
    //----------------------------------------------------------------
    if((code = dichotomy(-10.0,-5.0,1e-7, f2, &is_success, &result)) != OK)
        return show_error(code);
    if(is_success)
        printf("2x^3 - 3x^2 - 4 = 0, x in [-10;-5], x = %lf", result);
    else
        printf("2x^3 - 3x^2 - 4 = 0, x in [-10;-5], failed");
    printf("\n");
    //----------------------------------------------------------------
    if((code = dichotomy(-10.0,-5.0,-10, f2, &is_success, &result)) != OK)
        return show_error(code);
    if(is_success)
        printf("2x^3 - 3x^2 - 4 = 0, x in [-10;-5], x = %lf", result);
    else
        printf("2x^3 - 3x^2 - 4 = 0, x in [-10;-5], failed");
    printf("\n");
    //----------------------------------------------------------------
    return 0;
}