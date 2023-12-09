#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

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

const char* error_details[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Input error\n",
    "ERROR: Unknown...\n",
    "ERROR: Reserved...\n"
};

int show_error(status_code code);

typedef struct 
{
    int size;
    char* data;
} String, *String_ptr;

status_code string_create(const char* data, String_ptr result);

status_code string_create_from(const String_ptr str, String_ptr result);

void string_destroy(String_ptr ptr);

status_code string_concat(const String_ptr str1, const String_ptr str2, String_ptr result);

//First greater if failed
int string_compare(const void* str1v, const void* str2v);

bool string_equal(const String_ptr str1, const String_ptr str2);

status_code string_copy(String_ptr from_res, String_ptr to_res);
//Reading standart input
status_code string_readline(String_ptr res);

status_code string_readline_file(String_ptr res, FILE* in);

bool is_sep(char c);

status_code string_read_word(String_ptr res);

status_code string_read_word_file(String_ptr res, FILE* in);

status_code str_to_uint(String_ptr in, unsigned int* out, int base);

status_code str_to_int(String_ptr in, int* out, int base);

status_code str_to_d(String_ptr in, double* out, int base);

status_code sub_string(String_ptr source, int begin, int end, String_ptr result);