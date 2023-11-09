#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include "my_lib.h"

double calculate_poly(double* coefs, int max_power, double x)
{
    double result = 0;
    double x_pow = 1;
    for(int i = 0; i <= max_power; i++)
    {
        result = result + (coefs[i] * x_pow);
        x_pow = x_pow * x;
    }
    return result;
}

status_code poly_decompose(double eps, double a, double** result_coefs, int power, ...)
{
    if(power < 0)
        return INVALID_PARAMETER;
    

    double* begin_coefs_f = (double*) calloc(power + 1, sizeof(double));
    *result_coefs = (double*) calloc(power + 1, sizeof(double));

    if(begin_coefs_f == NULL || *result_coefs == NULL)
    {
        free_all(2, begin_coefs_f, *result_coefs);
        return ALLOC_ERROR;
    }
    va_list argptr;
    va_start(argptr, power + 1);
    for (int i = 0; i <= power; i++)
    {
        double x = va_arg(argptr, double);
        begin_coefs_f[i] = x;
    }
    va_end(argptr);

    double fact = 1;
    int curpower = power;
    for(int i = 0; i <= power; i++)
    {
        double res = calculate_poly(begin_coefs_f, curpower, a) / fact;
        (*result_coefs)[i] = res;

        //�����������
        curpower--;
        for(int j = 0; j <= curpower; j++)
        {
            double b = begin_coefs_f[j + 1];
            begin_coefs_f[j] = b * (j + 1);
        }
        fact *= (i + 1);
    }
    free(begin_coefs_f);
    return OK;
}

int main(int argc, char** argv)
{
    status_code code = OK;
    // test_arr1 = {-2, 1, -3, 0, 1};
    double* result;
    //-2 + x -3x^2 + 0x^3 + x^4
    if((code = poly_decompose(0.0001, 3, &result, 4, -2.0, 1.0, -3.0, 0.0, 1.0)) != OK)
        show_error(code);
    
    printf("Input coefs:");
    double test1[5] = {-2,1,3,0,1};
    print_double_arr(test1, 5, " ");
    printf("\n");
    printf("Ouptut coefs:");
    print_double_arr(result, 5, " ");
    printf("\n");

    // test_arr2 = {0,0,0,3};
    //0 + 0x + 0x^2 + 3x^3
    if((code = poly_decompose(0.0001, 3, &result, 3, 0.0, 0.0, 0.0, 3.0)) != OK)
        show_error(code);
    
    printf("Input coefs:");
    double test2[4] = {0.0, 0.0, 0.0, 3.0};
    print_double_arr(test2, 4, " ");
    printf("\n");
    printf("Ouptut coefs:");
    print_double_arr(result, 4, " ");
    printf("\n");
}