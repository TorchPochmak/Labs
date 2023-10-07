#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

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

static const char* usage = "Usage: <epsilon> <x>\n";

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
    "ERROR: Required: |x| < 1\n"
};

enum status_code bin_pow(ll a, ll n, ll* res) {
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

    if (!MY_OVERFLOW)
    {
        *res = *res * multiplier;
        return OK;
    }
    else
    {
        return MY_OVERFLOW;
    }
}

bool equal_d(double a, double b, double eps)
{
    return fabsl(a-b) < eps;
}

//--------------------

enum status_code sum1(double x, double* result, double EPS)
{
    if (EPS <= 0)
        return INVALID_PARAMETER;

    *result = exp(x);
    return OK;
}

enum status_code sum2(double x, double* result, double EPS)
{
    if (EPS <= 0)
        return INVALID_PARAMETER;
    //Вступайте в ряды Тейлора
    *result = cos(x);
    return OK;
}

enum status_code sum3(double x, double* result, double EPS)
{
    if (EPS <= 0)
        return INVALID_PARAMETER;
    //Да здравствует Даламбер
    if(fabs(x) > 1)
        return RESERVED;

    *result = 1; // n == 0

    double n = 1;
    double diff = -5;
    double last = 1;
    double fct = 0;
    while (!equal_d(last,0, EPS))
    {
        fct = (n - 1) * 3;
        diff = 27 * pow(n, 3) * pow(x,2) / ( (fct + 1) * (fct + 2) * (fct + 3) );
        last = last * diff;
        *result += last;
        n++;
    }
    return OK;
}

enum status_code sum4(double x, double* result, double EPS)
{
    if (EPS <= 0)
        return INVALID_PARAMETER;
    //Да здравствует Даламбер
    if(fabs(x) > 1)
        return RESERVED;

    *result = 0;

    double n = 1;
    double diff = -5;
    double last = 1;
    double fct = 0;
    while (!equal_d(last,0, EPS))
    {
        fct = (n - 1) * 2;
        diff = pow(x,2) * (fct + 1) / (fct + 2);
        last = last * (-diff);//no need in sign
        *result += last;
        n++;
    }
    return OK;
}

void print_res(enum status_code code, double res)
{
    if(code == OK)
        printf("%.9lf\n", res);
    else
        printf("%s", function_base_errors[code]);
}

int main(int argc, char** argv)
{
    printf("%s", usage);
    double EPS = 1e-8;

    if(argc != 3)
    {
        printf("%s", input_errors[1]);
        return 1;
    }
    double x = 0;
    double res = 0;

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
    //------------
    code = str_to_d(&argv[2], &x);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        return 1;
    }
    //-----------
    printf("A: ");
    code = sum1(x,&res, EPS);
    print_res(code, res);
    //
    printf("B: ");
    code = sum2(x, &res, EPS);
    print_res(code, res);
    //
    printf("C: ");
    code = sum3(x, &res, EPS);
    print_res(code, res);
    //
    printf("D: ");
    code = sum4(x, &res, EPS);
    print_res(code, res);
    
    return 0;
}