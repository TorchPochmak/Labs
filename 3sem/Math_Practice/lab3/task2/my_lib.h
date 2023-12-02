#ifndef MY_LIB
#define MY_LIB

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef unsigned long long int ull;
typedef long long int ll;
typedef unsigned char uchar;

#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)

#define MAX_INPUT_SIZE 1024 //Buffer size

//Error handler

typedef enum 
{
    OK,
    INVALID_PARAMETER,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    ALLOC_ERROR,
    FILE_ERROR,
    INPUT_ERROR,
    UNKNOWN_ERROR,
    RESERVED
} status_code;

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Input error\n"
    "ERROR: Unknown...\n",
    "ERROR: Reserved...\n"
};

int show_error(status_code code);

bool equal_d(double a, double b, double eps);

int get_digit(char c);

bool is_digit(char input, int base);

status_code str_to_uint(char** in, unsigned int* out, int base);

status_code str_to_int(char** in, int* out, int base);

status_code str_to_d(char** in, double* out);

status_code convert_base(char* input, int inputBase, int outputBase, char** result);

status_code read_string(char** res);

status_code my_strcat(char* in1, char* in2, char** out);

void fclose_all (int count, ...);

void free_all(int count, ...);

void print_double_arr(double* arr, int count, char* separator);

void print_int_arr(int* arr, int count, char* sep);

#endif