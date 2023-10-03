#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define ull unsigned long long int
#define ll long long int

enum status_code
{
    OK = 0,
    INVALID_PARAMETER,
    OVERFLOW,
    DIVISION_BY_ZERO,
    UNKNOWN_ERROR,
    RESERVED
};

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

status_code str_to_int(char* str, int* result)
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

status_code str_to_double(char* str, double* result)
{
    if (str == NULL)
    {
        *result = 0;
        return INVALID_PARAMETER;
    }
    int sdvig = 0;
    int dot_count = 0;

    if (str == "-" || str == "+")
    {
        *result = 0;
        return INVALID_PARAMETER;
    }
    if(str[0] == '-' || str[0] == '+')
    {
        sdvig = 1;
    }

    for (int i = sdvig; i < strlen(str); i++)
    {
        if(i == sdvig || i == (strlen(str) - 1))
        {
            if (!isdigit(str[i]))
                return INVALID_PARAMETER;
        }
        else{
            if (!isdigit(str[i]) && str[i] != '.')
                return INVALID_PARAMETER;
            if(str[i] == '.')
                dot_count++;
        }
    }
    if (dot_count > 1)
        return INVALID_PARAMETER;
    else
        *result = atof(str);
    return OK;
}

//----------------------------------------------------------------------------

static double EPS = 1e-7;

status_code function_m(int n1, int n2, bool* result)
{
    if (n1 == 0 || n2 == 0)
        return INVALID_PARAMETER;
    
    *result = (n1 % n2) == 0;
    return OK;
}

status_code function_t(double num1, double num2, double num3)
{
    
}

bool equal_arrays(double a[], double b[], int count)
{
    for(int i = 0; i < count; i++)
    {
        if(a[i] != b[i])
            return false;
    }
    return true;
}

bool equal(double a, double b)
{
    return fabs(a-b) < EPS;
}

int swap(int *a, int i, int j)
{
    if(a == NULL)
        return 1;

    int s = a[i];
    a[i] = a[j];
    a[j] = s;
    return 0;
}
bool next_generate(int *a, int n)
{
    if(a == NULL)
        return false;
    int j = n - 2;
    while (j != -1 && a[j] >= a[j + 1]) 
        j--;
    
    if (j == -1)
        return false;

    int k = n - 1;
    while (a[j] >= a[k]) 
        k--;

    swap(a, j, k);
    int l = j + 1, r = n - 1;
    while (l<r)
        swap(a, l++, r--);
    return true;
}


//if result_count == 1
//x1 == x2 == x
//if result_count == 2
//x1, x2
//else nothing
status_code solve_quad(double koefs[], int count, int* root_count, double* x1, double* x2)
{
    double D = koefs[1] * koefs[1] - 4 * koefs[0] * koefs[2];
    if(equal(koefs[0], D))
    {
        *root_count = -1;
        return OK;
    }
    if (equal(D,0))
    {
        *root_count = 1;
        *x1 = -koefs[1] / (4 * koefs[0] * koefs[2]);
        *x2 = *x1;
    }
    else if (D > 0)
    {
        *root_count = 2;
        *x1 = (-koefs[1] + sqrt(D)) / (4 * koefs[0] * koefs[2]);
        *x2 = (-koefs[1] - sqrt(D)) / (4 * koefs[0] * koefs[2]);
    }
    else
    {
        root_count = 0;
    }
    return OK;
}

status_code copy_to(double* from, double* to, int count)
{
    if(from == NULL || to == NULL)
        return INVALID_PARAMETER;
    for(int i = 0; i < count; i++)
    {
        to[i] = from[i];
    }
    return OK;
}

status_code change_indexes(double* to_change, double* indexes, int count, double* result)
{
    if (to_change == NULL || indexes == NULL)
        return INVALID_PARAMETER;
    for(int i = 0; i < count; i++)
    {
        result[i] = to_change[indexes[i]];
    }
    return OK;
}

status_code funciton_q(double num1, double num2, double num3)
{
    int count = 6;
    int list_len = 3;

    int indexes_list[] = {0,1,2};
    double list[] = {0};
    double last_list[] = {0};

    int root_count = 0;
    double x1 = 0, x2 = 0;

    for(int i = 0; i < 6; i++)
    {
        if(i == 0) 
        {
            list = {num1, num2, num3};
        }
        else
        {
            next_generate(indexes_list, 3);
            change_indexes(list, indexes_list, 3, list);
        }
        if (!equal_arrays(list, last_list, 3))
        {
            status_code code = solve_quad(list, 3, &root_count, &x1, &x2);
            if (code == OK)
            {
                if (root_count == -1)  
                    printf("It's not a quadratic equation with a = %lf, b = %lf, c = %lf", list[0], list[1], list[2]);
                if (root_count == 0)
                    printf("There are no real roots for equation with a = %lf, b = %lf, c = %lf", list[0], list[1], list[2]);
                if (root_count == 1)
                    printf("There is one root: x = %lf for equation with a = %lf, b = %lf, c = %lf", x1, list[0], list[1], list[2]);
                if (root_count == 2)
                    printf("There are two roots: x1 = %lf, x2 = %lf for equation with a = %lf, b = %lf, c = %lf", x1, x2, list[0], list[1], list[2]);
            }
            else
                return code;


        }
        copy_to(list, last_list);
    }
    return OK;
    
}

status_code solve_flag(int argc, char** argv)
{
    //-q eps num1 num2 num3
    //-m int1 int2
    //-t eps num1 num2 num3
    double d1 = 0, d2 = 0, d3 = 0;
    int n1 = 0, n2 = 0;

    switch(argv[1][1])
    {
        case 'q':
            if (argc != 6)
                return INVALID_PARAMETER;

            status_code code = str_to_double(argv[2], &EPS) & str_to_double(argv[3], &d1) &
                str_to_double(argv[4], &d2) & str_to_double(argv[4], &d3);

            if(code != OK)
                return code;

            code = funciton_q(d1, d2, d3);

            if(code != OK)
                return code;
            break;
        case 'm':
            if (argc != 4)
                return INVALID_PARAMETER;

            status_code code = str_to_int(argv[2], &n1) & str_to_int(argv[3], &n1);

            if(code != OK)
                return code;

            bool multiple = true;
            code = function_m(n1, n2, &multiple);

            if(multiple)
                printf("First number is a multiple for the second");
            else
                printf("First number is not multiple for the second");
        
            break;
        case 't':
            if (argc != 6)
                return INVALID_PARAMETER;
            
            status_code code = str_to_double(argv[2], &EPS) & str_to_double(argv[3], &d1) &
                str_to_double(argv[4], &d2) & str_to_double(argv[4], &d3);
            
            if(code != OK)
                return code;
            


            break;
    }

}

int main(int argc, char** argv)
{
    printf("%d", n);
    if (argc < 4)
    {
        printf(input_errors[1]);
        return 1;
    }



    if (strlen(argv[1]) == 2  && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
        if(solve_flag(argc,argv) != OK)
            return 1;
    }
    else
    {
        printf(input_errors[2]);
        return 1;
    }


    return 0;
}   