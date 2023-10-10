#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define ull unsigned long long int
#define ll long long int
#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)

enum status_code
{
    OK,
    INVALID_PARAMETER,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    UNKNOWN_ERROR,
    RESERVED,
};

enum status_code str_to_int(char** in, int* out)
{
    errno = 0;
    char* endptr = NULL;
    *out = strtol(*in, &endptr, 10);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(*in == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

enum status_code str_to_d(char** in, double* out)
{
    errno = 0;
    char* endptr = NULL;
    *out = strtod(*in, &endptr);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(*in == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;
    
    return OK;

}

bool equal_d(double a, double b, double eps)
{
    return fabsl(a-b) < eps;
}

static const char* usage = "Usage: <epsilon>\n";

static const char* input_errors[] =
{
    "ERROR: Wrong count of parameters\n",
    "ERROR: Unknown flag\n",
    "ERROR: Unknown input\n"
};

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: OVERFLOW\n",
    "ERROR: Division by zero\n",
    "ERROR: Unknown\n",
    "ERROR: Specific for function...\n"
};

//---------------
double integral1(double x) {
    //В пределе эта точка
    if(equal_d(x,0,1e-8)) 
        return 1;

    return log(1 + x) / x;
}

double integral2(double x) 
{
    return exp(-(x * x) / 2);
}

double integral3(double x) {

    //В пределе +inf, выкалываем и не учитываем
    if(equal_d(x,1,1e-8)) 
        return 0;

    return -log(1 - x);
}

double integral4(double x) {

    //В пределе эта точка
    if(equal_d(x,0,1e-8)) 
        return 1;

    return pow(x, x);
}


double integrate_eps(double(*f)(double), double a, double b, double eps)
{
    int n = 1; // Start with n = 1
    double res = 0.0, diff = 1 + eps;
    for (double h = (b - a); h > eps; h /= 2) {
        
        res = 0;
        for (int j = 0; j < n; j++) {
            double x = a + j * h;
            res += f(x) * h;
            diff = f(x) * h;
        }
        
        n *= 2;
    }

    return res;
}

int main(int argc, char** argv)
{
    printf("%s", usage);
    double EPS = 1e-8;
    if(argc != 2)
    {
        printf("%s", input_errors[1]);
        return 1;
    }

    //-----------
    enum status_code code = str_to_d(&argv[1], &EPS);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        return 1;
    }
    if (EPS < 1e-8)
    {
        printf("Incorrect epsilon. Must be greater than 1e-8\n");
        return 1;
    }
    printf("Integral of log(1 + x) / x\ta = 0,\tb = 1:\t\n%.9lf\n", integrate_eps(integral1, 0, 1, EPS));
    printf("Integral of exp(-(x * x) / 2)\ta = 0\tb = 1:\t\n%.9lf\n", integrate_eps(integral2, 0, 1, EPS));
    printf("Integral of -log(1 - x)\ta = 0\tb = 1:\t\n%.9lf\n", integrate_eps(integral3, 0, 1, EPS));
    printf("Integral of pow(x, x)\ta = 0\tb = 1:\t\n%.9lf\n", integrate_eps(integral4, 0, 1, EPS));

}