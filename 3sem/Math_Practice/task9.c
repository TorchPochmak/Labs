#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#define ull unsigned long long int
#define ll long long int
#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)
//#define max(x,y) ((x) > (y) ? (x) : (y))

enum status_code
{
    OK,
    INVALID_PARAMETER,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    UNKNOWN_ERROR,
    ALLOC_ERROR,
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

static const char* usage = "Usage: <input_file> <output_file>\n";

static const char* input_errors[] =
{
    "ERROR: Wrong count of parameters\n",
    "ERROR: Unknown flag\n",
    "ERROR: Unknown input\n",
    "ERROR: Cannot open file\n"
};

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: OVERFLOW\n",
    "ERROR: Division by zero\n",
    "ERROR: Unknown\n",
    "ERROR: Alloc error\n",
    "ERROR: Specific for function...\n"
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

int min(int a, int b)
{
    return a < b ? a : b;
}

//--------------------

const int ARR_SIZE = 20;

int get_rand(int a, int b)
{
    return a + rand() % (b - a + 1);
}

enum status_code print_arr(int* arr, int size)
{
    for(int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    return OK;
}

enum status_code task1_arr(int* arr)
{
    int minimum = INT_MAX;
    int maximum = INT_MIN;
    int mn_ind = -1;
    int mx_ind = -1;

    for (int i = 0; i < ARR_SIZE; i++)
    {
        if(arr[i] < minimum)
        {
            minimum = arr[i];
            mn_ind = i;
        }
        if(arr[i] > maximum)
        {
            maximum = arr[i];
            mx_ind = i;
        }
    }
    arr[mn_ind] = maximum;
    arr[mx_ind] = minimum;

    return OK;
}

enum status_code bin_search_closest(int* b, int size, int number, int* result)
{
    if(size == 0 || b == NULL)
        return INVALID_PARAMETER;

    int mid;
    int low = 0;
    int high = size;

    while (low < high) {
        mid = low + (high - low) / 2;

        if (number <= b[mid]) {
            high = mid;
        }
        else 
        {
            low = mid + 1;
        }
    }
    
    if(low >= size) {
       *result =  b[size - 1];
       return OK;
    }
    if(low < 0 || low == 0)
    {
        *result = b[0];
        return OK;
    }
    int right = b[low - 1];
    int left = b[low];
    if(abs(number - right) <= abs(number - left))
    {
        *result = right;
    }
    else
    {
        *result = left;
    }
    return OK;
}

//Компаратор для qsort
int cmp(const void *a, const void *b) {
     return *(int*)a - *(int*)b;
 }

int main(int argc, char** argv) 
{
    if (argc != 3)
    {
        printf("%s", input_errors[0]);
        return 1;
    }
    int alpha, beta;
    enum status_code code = OK;

    code = str_to_int(&argv[1], &alpha);

    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        return 1;
    }
    //------------
    code = str_to_int(&argv[2], &beta);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        return 1;
    }
    //-------------
    srand(time(NULL)); 

    int* arr = (int*) calloc(ARR_SIZE, sizeof(int));
    if(arr == NULL)
    {
        code = ALLOC_ERROR;
        printf("%s", function_base_errors[code]);
        return 1;
    }

    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = get_rand(alpha,beta);
    }
    printf("Task 1:\n");

    printf("Array before:\n");
    print_arr(arr, ARR_SIZE);

    task1_arr(arr);

    printf("Array after:\n");
    print_arr(arr, ARR_SIZE);

    //------------
    printf("\nTask 2:\n");

    int arr_size2 = get_rand(10,10000);

    int* a = (int*) calloc(arr_size2, sizeof(int));
    if(a == NULL)
    {
        code = ALLOC_ERROR;
        printf("%s", function_base_errors[code]);
        return 1;
    }
    int* b = (int*) calloc(arr_size2, sizeof(int));
    if(b == NULL)
    {
        code = ALLOC_ERROR;
        printf("%s", function_base_errors[code]);
        return 1;
    }
    int* c = (int*) calloc(arr_size2, sizeof(int));
    if(c == NULL)
    {
        code = ALLOC_ERROR;
        printf("%s", function_base_errors[code]);
        return 1;
    }



    for (int i = 0; i < arr_size2; i++)
    {
        a[i] = get_rand(-1000,1000);
        b[i] = get_rand(-1000,1000); 
    }
    printf("Array A:\n");
    print_arr(a, arr_size2);
    printf("Array B:\n");
    print_arr(b, arr_size2);
    printf("SORTED B:\n");
    qsort(b, arr_size2, sizeof(int), cmp);
    print_arr(b, arr_size2);
    printf("Array C:\n");
    

    for(int i = 0; i < arr_size2; i++)
    {
        int num = 0;
        code = bin_search_closest(b, arr_size2, a[i], &num);
        if(code != OK)
        {
            printf("%s", function_base_errors[code]);
            return 1;
        }
        c[i] = a[i] + num;
    }
    print_arr(c, arr_size2);
    
    free(a);
    free(b);
    free(c);

    return 0;
}