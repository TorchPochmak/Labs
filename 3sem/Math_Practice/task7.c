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
    UNKNOWN_ERROR,
    DIVISION_BY_ZERO,
    RESERVED,
    MY_OVERFLOW,
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
    
    if(!*endptr)
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
    
    if(!*endptr)
        return INVALID_PARAMETER;
    
    return OK;

}


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
    "ERROR: OVERFLOW\n",
    "ERROR: Division by zero\n",
    "ERROR: Unknown\n",
    "ERROR: Specific for function...\n"
};

enum status_code bin_pow(int a, int n, int* res) {
    if ((n == 0 && a == 0) || (n < 0) || (a == INT_MIN))
    {
        return INVALID_PARAMETER;
    }

    int multiplier = 1;
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

//multiples in range[1;100]
enum status_code function_h(int input)
{ 
    if (input < -100 || input > 100 || input == 0)
    {
        printf("There are no multiples for this number in range [1;100]");
    }
    input = abs(input);
    for (int i = input; i < 100; i += input)
    {
        printf("%d\n", i);
    }
    return OK;
}

//check prime
enum status_code function_p(int n, bool* result)
{
    if (n == 2)
    {
        *result = true;
        return OK;
    }
    if (n & 1 == 0) 
    {
        *result = false;
        return OK;
    }
    for (ll i = 3; i * i < n; i += 2) {
        if (n % i == 0)
        {
            *result = false;
            return OK;
        }
    }
    *result = true;
    return OK;
}

//print digits
enum status_code function_s(int input)
{
    if (input < 0){
        return INVALID_PARAMETER;
    }
    int digits_count = 0;
    int n = input;
    int digits_reversed[10] = { 0 };
    do
    {
        digits_reversed[digits_count] = n % 10;
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
enum status_code function_a(int n, int* result)
{
    bool overflow = false;
    if(n <= 0)
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
            overflow = overflow || mult(*result, n + 1, result);
        }
        else
        {
            //(n+1)/2*n
            overflow = overflow || plus(*result, 1, result);
            *result >>= 2;
            overflow = overflow || mult(*result, n, result);
        }
    }
    if (MY_OVERFLOW)
    {
        return OK;
    }
    else
    {
        return MY_OVERFLOW;
    }
}

//table of powers
enum status_code function_e(int n)
{
    //no need to check OVERFLOWSS
    if (n < 1 || n > 10)
    {
        return INVALID_PARAMETER;
    }
    printf("|\t\t\tbase\t\t\t|\t\t\tpower\t\t\t|\t\t\tresult\t\t\t|");
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= 10; j++)
        {
            int res = 0;
            bin_pow(i, j, &res);
            printf("|\t\t\t%d\t\t\t|\t\t\t%d\t\t\t|\t\t\t%d\t\t\t|", i, j, res);
        }
    }
    return OK;
}

//factorial
enum status_code function_f(int a, int* res)
{
    if (a < 0)
    {
        return INVALID_PARAMETER;
    }
    *res = 1;
    for (int i = 1; i <= a; i++)
    {
        if(mult(*res,a,res))
        {
            return MY_OVERFLOW;
        }
    }
    return OK;
}


enum status_code solve_flag(char flag, int number)
{
    enum status_code func_code = OK;
    //-h, -p, -s, -e, -a, -f
    switch (flag)
    {
        case 'h':
        {
            func_code = function_h(number);
            switch (func_code)
            {
                case OK:
                    break;
                default:
                    printf("%s", function_base_errors[func_code]);
                    break;
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
                    if (result == 0)
                        printf("Number is composite");
                    else
                        printf("Number if prime");
                    break;

                case INVALID_PARAMETER:
                    printf("Number is not prime and not composite");
                    break;

                default:
                    printf("%s", function_base_errors[func_code]);
                    break;
            }
            break;
        }
        case 's':
        {
            func_code = function_s(number);
            switch (func_code)
            {
                case OK:
                    break;

                case INVALID_PARAMETER:
                    printf("Number must be not negative");
                    break;

                default:
                    printf("%s", function_base_errors[func_code]);
                    break;
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
                    break;

                case INVALID_PARAMETER:
                    printf("Number must be in range [1;10]");
                    break;

                default:
                    printf("%s", function_base_errors[func_code]);
                    break;
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
                    break;

                default:
                    printf("%s", function_base_errors[func_code]);
                    break;
            }
            break;
        }
        case 'f':
        {
            int result = 0;
            func_code = function_f(number, &result);
            switch (func_code)
            {
                case OK:
                    break;

                case INVALID_PARAMETER:
                    printf("Factorial input must be a non-negative number");
                    break;

                default:
                    printf("%s", function_base_errors[func_code]);
                    break;
            }
            break;
        }
        default:
        {
            printf(input_errors[1]);
            func_code = UNKNOWN_ERROR;
            break;
        }
    }

    return func_code;
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
    
    enum status_code is_converted = str_to_int(&argv[1], &number);  
    switch (is_converted)
    {
        case OK:
            break;
        default:
            printf("%s", function_base_errors[is_converted]);
            return 1;
    }

    if (strlen(argv[1]) == 2 && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
        if(solve_flag(argv[1][1], number) != OK)
            return 1;
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
