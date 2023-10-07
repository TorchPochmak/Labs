#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define ull unsigned long long int
#define ll long long int

double MACHINE_EPS = -1;//required to calculate

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

bool lessequal_d(double a, double b, double eps)
{
    return equal_d(a,b,eps) || a < b;
}

double min(double a, double b)
{
    return a < b ? a : b;
}

double max(double a, double b)
{
    return a > b ? a : b;
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

//----------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
static double EPS = 1e-7;

enum status_code function_m(int n1, int n2, bool* result)
{
    if (n1 == 0 || n2 == 0)
        return INVALID_PARAMETER;
    
    *result = (n1 % n2) == 0;
    return OK;
}

enum status_code function_t(double a, double b, double c, double eps, bool* result)
{
    if (eps <= 0)
        return INVALID_PARAMETER;
    double mn = 0, middle = 0, mx = 0;
    mn = min(a,min(b,c));
    mx = max(a,max(b,c));
    middle = a + b + c - mn - mx;

    if(lessequal_d(mn, 0, eps))
        return INVALID_PARAMETER;
    
    if(equal_d(mn * mn + middle * middle, mx * mx, eps))
    {
        *result = true;
        return OK;
    }
    else
    {
        *result = false;
        return OK;
    }
}

//if result_count == 1
//x1 == x2 == x
//if result_count == 2
//x1, x2
//else nothing
enum status_code solve_quad(double koefs[], int count, double EPS, int* root_count, double* x1, double* x2)
{
    if (EPS <= 0)
        return INVALID_PARAMETER;

    double D = koefs[1] * koefs[1] - 4 * koefs[0] * koefs[2];
    if(equal_d(koefs[0], 0, EPS))
    {
        *root_count = -1;
        return OK;
    }
    if (equal_d(D,0, EPS))
    {
        *root_count = 1;
        *x1 = -koefs[1] / (2 * koefs[0]);
        *x2 = *x1;
    }
    else if (D > 0)
    {
        *root_count = 2;
        *x1 = (-koefs[1] + sqrt(D)) / (2 * koefs[0]);
        *x2 = (-koefs[1] - sqrt(D)) / (2 * koefs[0]);
    }
    else
    {
        *root_count = 0;
    }
    return OK;
}

enum status_code copy_to(double* from, double* to, int count)
{
    if(from == NULL || to == NULL)
        return INVALID_PARAMETER;
    for(int i = 0; i < count; i++)
    {
        to[i] = from[i];
    }
    return OK;
}

enum status_code change_indexes(double* to_change, int* indexes, int count, double* result)
{
    double res[3] = {0};
    if (to_change == NULL || indexes == NULL)
        return INVALID_PARAMETER;
    for(int i = 0; i < count; i++)
    {
        res[i] = to_change[indexes[i]];
    }
    for(int i = 0; i < count; i++)
    {
        result[i] = res[i];
    }
    return OK;
}

enum status_code funciton_q(double num1, double num2, double num3, double EPS)
{
    if (EPS <= 0)
        return INVALID_PARAMETER;

    int count = 6;
    int list_len = 3;
    double readonly_list[3] = {num1, num2, num3};

    int indexes_list[] = {0,1,2};
    double list[3] = {num1, num2, num3};
    double last_list[6][3] = {0};

    int root_count = 0;
    double x1 = 0, x2 = 0;

    int unique_arrays = 0;
    for(int i = 0; i < 6; i++)
    {
        if(i != 0)
        {
            next_generate(indexes_list, 3);
            change_indexes(readonly_list, indexes_list, 3, list);
        }
        bool is_unique = true;
        for(int i = 0; i < unique_arrays; i++)
        {
            if(equal_arrays(list, last_list[i], 3))
                is_unique = false;
        }
        if (is_unique)
        {
            unique_arrays++;
            copy_to(list, last_list[unique_arrays - 1], 3);
            enum status_code code = solve_quad(list, 3, EPS, &root_count, &x1, &x2);
            if (code == OK)
            {
                if (root_count == -1)  
                    printf("It's not a quadratic equation with a = %.3lf, b = %.3lf, c = %.3lf\n", list[0], list[1], list[2]);
                if (root_count == 0)
                    printf("There are no real roots for equation with a = %.3lf, b = %.3lf, c = %.3lf\n", list[0], list[1], list[2]);
                if (root_count == 1)
                    printf("There is one root: x = %.3lf for equation with a = %.3lf, b = %.3lf, c = %.3lf\n", x1, list[0], list[1], list[2]);
                if (root_count == 2)
                    printf("There are two roots: x1 = %.3lf, x2 = %.3lf for equation with a = %.3lf, b = %.3lf, c = %.3lf\n", x1, x2, list[0], list[1], list[2]);
            }
            else
                return code;
        }
    }
    return OK;
    
}

enum status_code solve_flag(int argc, char** argv)
{
    enum status_code code = OK;
    //-q eps num1 num2 num3
    //-m int1 int2
    //-t eps num1 num2 num3
    double d1 = 0, d2 = 0, d3 = 0;
    int n1 = 0, n2 = 0;

    switch(argv[1][1])
    {
        case 'q':
        {
            if (argc != 6)
                return INVALID_PARAMETER;
            //------------
            code = str_to_d(&argv[2], &EPS);
            if(code != OK)
                return code;
            //------------
            
            //------------
            code = str_to_d(&argv[5], &d3);
            if(code != OK)
                return code;
            //------------
            code = funciton_q(d1, d2, d3, EPS);
            if(code != OK)
                return code;
            //------------
            break;
        }
        case 'm':
        {
            if (argc != 4)
                return INVALID_PARAMETER;
            //-------------
            code = str_to_int(&argv[2], &n1);
            if(code != OK)
                return code;
            //-------------
            code = str_to_int(&argv[3], &n2);
            if(code != OK)
                return code;
            //-------------
            bool multiple = true;
            code = function_m(n1, n2, &multiple);

            if(code != OK)
                return code;
            
            if(multiple)
                printf("First number is a multiple for the second");
            else
                printf("First number is not multiple for the second");
        
            break;
        }
        case 't':
        {
            bool result = true;
            if (argc != 6)
                return INVALID_PARAMETER;
            
            //------------
            code = str_to_d(&argv[2], &EPS);
            if(code != OK)
                return code;
            //------------
            code = str_to_d(&argv[3], &d1);
            if(code != OK)
                return code;
            //------------
            code = str_to_d(&argv[4], &d2);
            if(code != OK)
                return code;
            //------------
            code = str_to_d(&argv[5], &d3);
            if(code != OK)
                return code;
            
            code = function_t(d1,d2,d3,EPS,&result);

            if(code != OK)
                return code;

            if(result)
                printf("Yes, %lf, %lf and %lf can form a right triangle\n", d1,d2,d3);
            else
                printf("No, %lf, %lf and %lf can't form a right triangle\n", d1,d2,d3);
    
            break;
        }
    }
    
    return code;
}

int main(int argc, char** argv)
{
    // argc = 6;
    // argv[1] = "-q";
    // argv[2] = "1e-5";
    // argv[3] = "3";
    // argv[4] = "4";
    // argv[5] = "5";
    if (argc < 4)
    {
        printf(input_errors[1]);
        return 1;
    }

    if (strlen(argv[1]) == 2  && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
        enum status_code st = solve_flag(argc,argv);
        if(st != OK)
        {
            printf("%s", function_base_errors[st]);
            return 1;
        }
    }
    else
    {
        printf(input_errors[2]);
        return 1;
    }


    return 0;
}   