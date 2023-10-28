#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "my_lib.h"


typedef struct 
{
    double x, y;
} vec2d;

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

//работаем в даблах
//cross(a,b) == len(a) * len(b) * sin(angle(a,b)) == формула ниже
int cross_sign(vec2d a, vec2d b) 
{
    double res = a.x * b.y - a.y * b.x;
    if(equal_d(res, 0, EPS))
        return 0;
    return res > 0 ? 1 : -1;
}

status_code is_convex_polygon(bool* result, int count, ...)
{
    if(count == 0 || count % 2 != 0 || count < 6)
        return INVALID_PARAMETER;

    va_list argptr;
    va_start(argptr, count);


    //проверить угол для всех соседних троек 

    //Синусы всех углов одного знака должны быть одного знака (все правые повороты, все левые повороты)
    //выражение через cross, длина в любом случае ненулевая, так что важен только знак
    vec2d first, second;
    int cross_sign1, cross_sign2;
    double x1,y1,x2,y2,x3,y3;

    double x_first, y_first;
    double x_second, y_second;
    
    x1 = va_arg(argptr,double);
    y1 = va_arg(argptr,double);
    x2 = va_arg(argptr,double);
    y2 = va_arg(argptr,double);
    x3 = va_arg(argptr,double);
    y3 = va_arg(argptr,double);
    first.x = x1 - x2;
    first.y = y1 - y2;
    second.x = x3 - x2;
    second.y = y3 - y2;

    x_first = x1;
    y_first = y1;
    x_second = x2;
    y_second = y2;

    cross_sign1 = cross_sign(first, second);
    for(int i = 0; i < count - 6; i += 2)
    {
        //shift x and y
        x1 = x2;
        y1 = y2;

        x2 = x3;
        y2 = y3;

        x3 = va_arg(argptr,double);
        y3 = va_arg(argptr,double);

        first.x = x1 - x2;
        first.y = y1 - y2;
        second.x = x3 - x2;
        second.y = y3 - y2;

        cross_sign2 = cross_sign(first, second);
        if(cross_sign1 * cross_sign2 <= 0)
        {
            *result = false;
            va_end(argptr);
            return OK;
        }
        cross_sign1 = cross_sign2;
    }
    //нужно еще проверить x2,y2, x3,y3, first_x, first_y
    //нужно еще проверить x3,y3, first_x, first_y, second_x, second_y

    first.x = x2 - x3;
    first.y = y2 - y3;
    second.x = x_first - x3;
    second.y = y_first - y3;
    cross_sign2 = cross_sign(first, second);

    if(cross_sign1 * cross_sign2 <= 0)
    {
        *result = false;
        va_end(argptr);
        return OK;
    }

    cross_sign1 = cross_sign2;

    first.x = x3 - x_first;
    first.y = y3 - y_first;
    second.x = x_second - x_first;
    second.y = y_second - y_first;
    cross_sign2 = cross_sign(first, second);

    if(cross_sign1 * cross_sign2 <= 0)
    {
        *result = false;
        va_end(argptr);
        return OK;
    }

    va_end(argptr);
    *result = true;
    return OK;
}

//От старшего до младщего коэфа
status_code calc_poly(double* result, double x, int n, ...)
{
    if(n < 0)
        return INVALID_PARAMETER;
    va_list argptr;
    va_start(argptr, n + 1);
    double* coefs = (double*) calloc(n + 1, sizeof(double));
    for(int i = 0; i <= n; i++)
    {  
        coefs[n - i] = va_arg(argptr, double);
    }
    va_end(argptr);
    double x_cur = x;
    *result = coefs[0];
    for(int i = 1; i <= n; i++)
    {
        *result += coefs[i] * x_cur;
        x_cur *= x;
    }
    free(coefs);
    return OK;
}
// треугольник 1.0, 1.0, 3.0, 1,0, 2.0, 2.0
// невыпуклый пятиугольник 0.0,0.0, 0.0, 1.0, 0.25, 0.25, 1.0, 0.0

int main() {
    calc_eps();
    bool res = false;
    status_code code = OK;

    if((code = is_convex_polygon(&res, 8, 0.0,0.0, 0.0, 1.0, 0.25, 0.25, 1.0, 0.0)) != OK)
        show_error(code);

    printf("First polygon test: %d\n", res);

    res = false;
    if((code = is_convex_polygon(&res, 6, 1.0, 1.0, 3.0, 1,0, 2.0, 2.0)) != OK)
        show_error(code);
    printf("Second polygon test: %d\n", res);
    //

    double res1 = 0;
    //x = -3
    //x^2 + 5x + 10 = 9 - 15 + 10 = 4;
    if((code = calc_poly(&res1, -3, 2, 1.0, 5.0, 10.0)) != OK)
        show_error(code);
    printf("First polynom test: %lf\n", res1);
    //

    res1 = 0;
    //x = -3
    //10 = 10;
    if((code = calc_poly(&res1, -3, 0, 10.0)) != OK)
        show_error(code);
    printf("Second polynom test: %lf\n", res1);
    return OK;
}