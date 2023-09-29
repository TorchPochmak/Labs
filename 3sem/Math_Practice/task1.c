#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define ull unsigned long long int
#define ll long long int

enum functions_status_codes
{
    OK = 0,
    INVALID_PARAMETER,
    OVERFLOW,
    DIVISION_BY_ZERO,
    UNKNOWN_ERROR,
    RESERVED
};

static const char* usage = "Usage:\n -<flag> <number> or\n /<flag> <number>\n flags: -h, -p, -s, -e, -a, -f\n ";

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
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Unknown\n",
    "ERROR: Specific for function"
};

functions_status_codes str_to_int(char* str, int* result)
{
    if (str == NULL)
    {
        *result = 0;
        return INVALID_PARAMETER;
    }
    if (str == "-" || str == "+")
    {
        *result = 0;
        return INVALID_PARAMETER;
    }

    int count = strlen(str);
    //check if is number
    bool check_first = (isdigit(str[0]) || str[0] == '-' || str[0] == '+');
    for (int i = 0; i < count; i++)
    {
        if (!check_first || (i != 0 && !isdigit(str[i])))
        {
            *result = 0;
            return INVALID_PARAMETER;
        }
    }

    //-[INT_MAX;INT_MAX]
    *result = 0;
    int digits = 0;
    bool is_counting_zeros = false;
    int multiplier = 1;

    if (str[0] == '-')
    {
        multiplier = -1;
    }
    for (int i = 0; i < count; i++)
    {
        if(i == 0 && (str[i] == '+' || str[i] == '-'))
        {
            continue;
        }
        if (str[i] != '0')
        {
            is_counting_zeros = true;
        }
        if (is_counting_zeros)
        {
            digits++;
            if (digits == 1)
            {
                *result = multiplier * (str[i] - '0');
            }
            else
            {
                if (digits < 10)
                {
                    *result = *result * 10 + (multiplier) * (str[i] - '0');
                }

                if (digits == 10)
                {
                    if (abs(*result) > INT_MAX / 10)
                    {
                        *result = 0;
                        return OVERFLOW;
                    }
                    else if (abs(*result) == INT_MAX / 10)
                    {
                        //INT_MAX mod 10 == 7
                        //INT_MIN mod 10 == 8
                        if ( ((str[i] - '0') <= 7 && multiplier == 1) || ((str[i] - '0') <= 8 && multiplier == -1) )
                        {
                            *result = *result * 10 + (multiplier) * (str[i] - '0');
                        }
                        else
                        {
                            *result = 0;
                            return OVERFLOW;
                        }
                    }
                    else
                    {
                        *result = *result * 10 + (multiplier) * (str[i] - '0');
                    }
                }

                if (digits > 10)
                {
                    *result = 0;
                    return OVERFLOW;
                }
            }
        }

    }
    if (digits == 0)
    {
        *result = 0;
    }
    return OK;
}

//--------------------------------------------------------------------------------------

//using these if there is a risk of overflow
bool mult_of(int a, int b, int* result)
{
    *result = a * b;
    if (a != 0 && *result / a != b)
    {
        return false;
    }
    return true;
}
bool sum_of(int a, int b, int* result)
{
    *result = a + b;
    if (a > 0 && b > INT_MAX - a) {
        return false;
    } else if (a < 0 && b < INT_MIN - a) {
        return false;
    }
    return true;
}
bool minus_of(int a, int* result)
{
    if (a == INT_MIN)
    {
        return false;
    }
    else
    {
        *result = -a;
        return true;
    }
}

//а где templates... :(
bool multll_of(ll a, ll b, ll* result)
{
    *result = a * b;
    if (a != 0 && *result / a != b)
    {
        return false;
    }
    return true;
}
bool sumll_of(ll a, ll b, ll* result)
{
    *result = a + b;
    if (a > 0 && b > INT_MAX - a) {
        return false;
    } else if (a < 0 && b < INT_MIN - a) {
        return false;
    }
    return true;
}
bool minusll_of(ll a, ll* result)
{
    if (a == INT_MIN)
    {
        return false;
    }
    else
    {
        *result = -a;
        return true;
    }
}

//--------------------------------------------------------------------------------------

functions_status_codes bin_pow(ll a, ll n, ll* res) {
    if ((n == 0 && a == 0) || (n < 0))
    {
        return INVALID_PARAMETER;
    }

    int multiplier = 1;
    bool no_overflow = true;
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
            no_overflow = no_overflow & multll_of(*res, a, res);
        }
        no_overflow = no_overflow & multll_of(a,a,&a);
        n >>= 1;
    }

    if (no_overflow)
    {
        *res = *res * multiplier;
        return OK;
    }
    else
    {
        return OVERFLOW;
    }
}

//multiples in range[1;100]
functions_status_codes function_h(int input)
{ 
    if (input < -100 || input > 100 || input == 0)
    {
        printf("There are no multiples for this number in range [1;100]");
    }
    input = abs(input);
    for (int i = input; i < 100; i += input)
    {
        printf("&d\n", i);
    }
    return OK;
}

//check prime
functions_status_codes function_p(int n, bool result)
{
    if (n == 2)
    {
        result = true;
        return OK;
    }
    if (n & 1 == 0) 
    {
        result = false;
        return OK;
    }
    for (int i = 3; i * i < n; i += 2) {
        if (n % i == 0)
        {
            result = false;
            return OK;
        }
    }
    result = true;
    return OK;
}

//print digits
functions_status_codes function_s(int input)
{
    if (input < 0){
        return INVALID_PARAMETER;
    }
    int digits_count = 0;
    int n = input;
    int digits_reversed[10] = { 0 };
    do
    {
        digits_reversed[digits_count] = n & 1;
        digits_count++;
        n >>= 2;
    } while (n & 1);

    for(int i = digits_count - 1; i >= 0; i--)
    {
        printf("%d ", digits_reversed[i]);
    }
    return OK;
}

//sum 1_to_n
functions_status_codes function_a(int n, int* result)
{
    bool no_overflow = true;
    if(n <= 0 || n == INT_MAX)
    {
        return INVALID_PARAMETER;
    }
    else
    {
        *result = n;
        if(n & 1 == 0)
        {
            //n/2*(n+1)
            *result >>= 1;
            no_overflow = no_overflow & mult_of(*result, n + 1, result);
        }
        else
        {
            //(n+1)/2*n
            *result += 1;
            *result >>= 2;
            no_overflow = no_overflow & mult_of(*result, n, result);
        }
    }
    if (no_overflow)
    {
        return OK;
    }
    else
    {
        return OVERFLOW;
    }
}

//table of powers
functions_status_codes function_e(int n)
{
    //no need to check overflow
    if (n < 1 || n > 10)
    {
        return INVALID_PARAMETER;
    }
    printf("|\t\t\tbase\t\t\t|\t\t\tpower\t\t\t|\t\t\tresult\t\t\t|");
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= 10; j++)
        {
            ll res = 0;
            bin_pow(i, j, &res);
            printf("|\t\t\t%d\t\t\t|\t\t\t%d\t\t\t|\t\t\t%d\t\t\t|", i, j, res);
        }
    }
    return OK;
}

//factorial
functions_status_codes function_f(ll a, ll* res)
{
    if (a < 0)
    {
        return INVALID_PARAMETER;
    }
    *res = 1;
    for (int i = 1; i <= a; i++)
    {
        if(!multll_of(*res,a,res))
        {
            return OVERFLOW;
        }
    }
    return OK;
}

int main(int argc, char** argv)
{
    printf(usage);
    if (argc != 3)
    {
        printf(input_errors[1]);
        return 1;
    }

    int number = 0;
    functions_status_codes is_converted = str_to_int(argv[1], &number);  
    switch (is_converted){
        case OK:
        {
            break;
        }
        default:
        {
            printf("%s", function_base_errors[is_converted]);
            return 1;
        }
    }

    if (strlen(argv[1]) == 2 && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
        functions_status_codes func_code = OK;
        //-h, -p, -s, -e, -a, -f
        switch (argv[1][1])
        {
            case 'h':
            {
                func_code = function_h(number);
                switch (func_code)
                {
                    case OK:
                    {
                        break;
                    }
                    default:
                    {
                        printf("%s", function_base_errors[func_code]);
                        return 1;
                    }
                }
                break;
            }
            case 'p':
            {
                bool result = 0;
                func_code = function_p(number, &result);
                switch (func_code)
                {
                    case OK:
                    {
                        if (result == 0)
                        {
                            printf("Number is composite");
                        }
                        else
                        {
                            printf("Number if prime");
                        }
                        break;
                    }
                    case INVALID_PARAMETER:
                    {
                        printf("Number is not prime and not composite");
                        return 1;
                    }
                    default:
                    {
                        printf("%s", function_base_errors[func_code]);
                        return 1;
                    }
                }
                break;
            }
            case 's':
            {
                func_code = function_s(number);
                switch (func_code)
                {
                    case OK:
                    {
                        break;
                    }
                    case INVALID_PARAMETER:
                    {
                        printf("Number must be not negative");
                        break;
                    }
                    default:
                    {
                        printf("%s", function_base_errors[func_code]);
                        return 1;
                    }
                }
                break;
            }
            case 'e':
            {
                int result = 0;
                func_code = function_e(result);
                switch (func_code)
                {
                    case OK:
                    {
                        break;
                    }
                    case INVALID_PARAMETER:
                    {
                        printf("Number must be in range [1;10]");
                        break;
                    }
                    default:
                    {
                        printf("%s", function_base_errors[func_code]);
                        return 1;
                    }
                }
                break;
            }
            case 'a':
            {
                int result = 0;
                func_code = function_a(number, &result);
                switch (func_code)
                {
                    case OK:
                    {
                        break;
                    }
                    default:
                    {
                        printf("%s", function_base_errors[func_code]);
                        return 1;
                    }
                }
                break;
            }
            case 'f':
            {
                ll result = 0;
                func_code = function_f(number, &result);
                switch (func_code)
                {
                    case OK:
                    {
                        break;
                    }
                    case INVALID_PARAMETER:
                    {
                        printf("Factorial input must be a non-negative number");
                        break;
                    }
                    default:
                    {
                        printf("%s", function_base_errors[func_code]);
                        return 1;
                    }
                }
                break;
            }
            default:
            {
                printf(input_errors[1]);
                return 1;
            }
        }
    }
    else
    {
        printf(input_errors[2]);
        return 1;
    }

    printf("Print any char to close the program");
    getchar();
    return 0;
}
