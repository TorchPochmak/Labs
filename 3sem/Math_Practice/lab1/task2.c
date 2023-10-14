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

enum status_code factorial(ll a, double* res)
{
    if (a < 0)
    {
        return INVALID_PARAMETER;
    }
    *res = 1;
    for (ll i = 1; i <= a; i++)
    {
        *res *= (double)i;
    }
    return OK;
}

enum status_code combination(ll n, ll k, double* res)
{
    if(n < 0 || k < 0 || n < k)
    {
        return INVALID_PARAMETER;
    }
    *res = 1;
    for(ll i = k + 1; i <= n; i++)
    {
         *res *= (double)i;
    }
    double res2 = 1;
    enum status_code f = factorial(n - k, &res2);
    if(f != OK)
    {
        return f;
    }
    *res = *res / res2;
    return OK;
}

//----------------------

double e_lim(double EPS) 
{
    ll n = 1;
    double prev = 1.0;
    double cur = 0;

    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        cur = pow(1 + 1.0 / n, n);
        n++;
    }
    return cur;
}

double pi_lim(double EPS)
{
    ll n = 1;
    double prev = 0;
    double cur = 4.0;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        // x[n + 1] / x[n] = (4n^2 + 4n) / ((2n + 1)^2)
        cur *=  ((4 * n * n + 4 * n) / pow(2 * n + 1,2));
        n++;
    }
    return cur;
}

double ln2_lim(double EPS)
{
    ll n = 1;
    double prev = 1.0;
    double cur = 0;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        cur =  n * (pow(2, 1.0/n) - 1);
        n++;
    }
    return cur;
}

double sqrt2_lim(double EPS)
{
    ll n = 1;
    double prev = -0.5;
    double cur = 0;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        cur =  prev - (prev * prev) / 2.0 + 1;
        n++;
    }
    return cur;
}

double gamma_lim(double EPS)
{
    //Иначе сходится слишком долго
    if (EPS < 1e-3) EPS = 1e-3;

    ll n = 1;
    double prev = 0;
    double cur = 1.0;
    double fact = 1;
    int mult = -1;
    double comb;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        cur = 0;
        fact = 1;
        mult = -1;
        for(int i = 1; i <= n; i++)
        {
            //факториал можно на ходу считать
            //invalid parameter не случится
            fact = fact * i;            
            combination(n, i, &comb);
            cur += (comb * mult * log(fact)) / (double)i;
            mult = mult * -1;
        }
        n++;
    }
    return cur;
}

//----------------------

double e_sum(double EPS)
{
    ll n = 1;
    double prev = 1.0;
    double cur = 0;
    double fact = 1;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        fact = fact * n;
        cur += 1.0 / fact; 
        n++;
    }
    return cur + 1;
}

double pi_sum(double EPS)
{
    double n = 1;
    double prev = 1.0;
    double cur = 0;
    int mult = 1;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;

        cur += mult / (2 * n - 1);
        n++;
        mult = -mult;
    }
    return 4 * cur;
}

double ln2_sum(double EPS)
{
    double n = 1;
    double prev = 1.0;
    double cur = 0;
    int mult = 1;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;

        cur += mult / n;
        n++;
        mult = -mult;
    }
    return cur;
}

double sqrt2_sum(double EPS)
{
    double p = 0.25; // 2 * (-n), where n == 2
    double prev = 0.0;
    double cur = pow(2, p);
    int mult = 1;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        p = p * 0.5; // 2 * -(n + 1)
        cur = cur * pow(2, p);
        mult = -mult;
    }
    return cur;
}

double gamma_sum(double EPS)
{
    double prev = 1;
    double cur = 0;
    double x = 0;
    int k = 2;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        x = pow(floor(sqrt(k)), 2);
        if(!equal_d(x, k, 1e-10))
        {
            cur += 1.0 / x - 1.0 / (double)k;
        }
        else 
        {
            prev = INT_MAX;//Нужно, чтобы сумма обнулилась дважды
        }
        k++;
    }
    double pi = acos(-1.0);
    return -pi * pi / 6 + cur;
}

//---------------------

double e_equation(double EPS)
{
    double prev = 0; 
	double cur = 1;
	do
	{
		prev = cur;	
		// f(x) = log(x) - 1
		// f(x) / f'(x) = x * (log(x) - 1)
		cur -= cur*(log(cur)-1);
	} while (fabsl(cur - prev) > EPS);
	return cur;
}

double pi_equation(double EPS)
{
	double prev = 0; 
	double cur = 1;
	do
	{
		prev = cur;	
		cur += (cos(cur)+1) / sin(cur);
	} while (fabsl(cur - prev) > EPS);
	return cur;
}

double ln2_equation(double EPS)
{
	double prev = 0; 
	double cur = 1;
	do
	{
		prev = cur;	
		cur -= 1 - 2/exp(cur);
	} while (fabsl(cur - prev) > EPS);
	return cur;
}

double sqrt2_equation(double EPS)
{
    double prev = 0; 
	double cur = 0.5;
	do
	{
		prev = cur;
		cur += -cur * cur / 2 + 1;
	} while (fabsl(cur - prev) > EPS);
	return cur;
}

//check prime
bool check_prime(int n)
{
    if (n < 2)
    {
        return false;
    }
    if (n == 2)
    {
        return true;
    }
    if (n % 2 == 0) 
    {
        return false;
    }
    for (ll i = 3; i * i <= n; i += 2) {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

double gamma_equation(double EPS)
{
    //Иначе слишком долго
    if(EPS < 1e-5) EPS = 1e-5;

    int t = 1;
    double prev = 0;
    double cur = 1;
    while (!equal_d(prev, cur, EPS))
    {
        prev = cur;
        double multiplication = 1;
        for(int i = 2; i <= t; i++)
        {
            if(check_prime(i))
            {
                multiplication *= ((double)i - 1.0) / (double)i;
            }
        }
        cur = multiplication * log(t);
        t++;
    }
    return -log(cur);
}

int main(int argc, char** argv)
{   
    double EPS = 1e-8;

    printf(usage);
    if (argc != 2)
    {
        printf(input_errors[1]);
        return 1;
    }

    //default EPS = 1e-6
    enum status_code is_converted = str_to_d(&argv[1], &EPS);  
    switch (is_converted)
    {
        case OK:
            break;
        default:
            printf("%s", function_base_errors[is_converted]);
            return 1;
    }
    if (EPS < 1e-8)
    {
        printf("Incorrect epsilon. Must be greater than 1e-8\n");
        return 1;
    }
    printf("Constant e\tLimit: %.9lf\tInfinite multiplication/sum: %.9lf\tEquation: %.9lf\n", e_lim(EPS), e_sum(EPS), e_equation(EPS));
    printf("Constant pi\tLimit: %.9lf\tInfinite multiplication/sum: %.9lf\tEquation: %.9lf\n", pi_lim(EPS), pi_sum(EPS), pi_equation(EPS));
    printf("Constant ln2\tLimit: %.9lf\tInfinite multiplication/sum: %.9lf\tEquation: %.9lf\n", ln2_lim(EPS), ln2_sum(EPS), ln2_equation(EPS));
    printf("Constant sqrt2\tLimit: %.9lf\tInfinite multiplication/sum: %.9lf\tEquation: %.9lf\n", sqrt2_lim(EPS), sqrt2_sum(EPS), sqrt2_equation(EPS));
    printf("Constant gamma\tLimit: %.9lf\tInfinite multiplication/sum: %.9lf\tEquation: %.9lf\n", gamma_lim(EPS), gamma_sum(EPS), gamma_equation(EPS));
}
