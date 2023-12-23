#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
//#include "String.h"

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

typedef struct 
{
    int size;
    char* data;
} String, *String_ptr;

int show_error(status_code code)
{
    printf("%s", error_details[code]);
    return code;
}

status_code string_create(const char* data, String_ptr result)
{
    String res;
    if(data == NULL)
    {
        res.size = 0;
        res.data = NULL;
        *result = res;
        return OK;
    }
    res.size = strlen(data);
    res.data = (char*) malloc(sizeof(char) * (res.size + 1));
    if(!(res.data))
        return ALLOC_ERROR;
    strcpy(res.data, data);
    *result = res;
    return OK;
}

//dynamic
status_code copy_to_dynamic(const String_ptr str, String_ptr* result)
{
    String_ptr res = (String_ptr) malloc(sizeof(String));
    if(!res)
        return ALLOC_ERROR;
    if(str == NULL)
        return INVALID_PARAMETER;
    res->data = str->data;
    res->size = str->size;
    *result = res;
    return OK;
}

void string_destroy(String_ptr ptr)
{
    if(ptr == NULL)
        return;
    if(ptr->data != NULL)
        free(ptr->data); 
    ptr->data = NULL;
    ptr = NULL;
}

status_code string_concat(const String_ptr str1, const String_ptr str2, String_ptr result)
{
    if(str1 == NULL || str2 == NULL || str1->data == NULL || str2->data == NULL)
        return INVALID_PARAMETER;
    String res;
    int n1 = str1->size;
    int n2 = str2->size;
    res.size = n1 + n2 + 1;
    res.data = (char*) malloc(sizeof(char) * (n1 + n2 + 1));
    if(res.data == NULL)
    {
        string_destroy(&res);
        return ALLOC_ERROR;
    }
    for(int i = 0; i < n1; i++)
    {
        res.data[i] = str1->data[i];
    }
    for(int i = 0; i < n2; i++)
    {
        res.data[i + n1] = str2->data[i];
    }
    res.data[n1 + n2] = '\0';
    *result = res;
    return OK;
}
//First greater if failed
int string_compare(const void* str1v, const void* str2v)
{
    String* str1 = (String*) str1v;
    String* str2 = (String*) str2v;
    if(str1 == NULL && str2 == NULL || (str1->data == NULL && str2->data == NULL)) 
        return 0;
    if(str1 == NULL || str2 == NULL || str1->data == NULL || str2->data == NULL)
        return 1;
    int res = str1->size == str2->size ? 0 : 
        (str1->size - str2->size > 0 ? 1 : -1);
    if(res == 0)
    {
        int i = 0;
        while(i != str1->size && str1->data[i] == str2->data[i])
            i++;
        if(i == str1->size)
            return 0;
        return str1->data[i] - str2->data[i] > 0 ? 1 : -1;
    }
    return res;
}

bool string_equal(const String_ptr str1, const String_ptr str2)
{
    return string_compare((void*)str1, (void*)str2) == 0 ? true : false;
}

status_code string_copy(String_ptr from_res, String_ptr to_res)
{
    String to;
    if(from_res == NULL)
    {
        return INVALID_PARAMETER; 
    }
    if(from_res->data == NULL)
    {
        to.size = 0;
        to.data = NULL;
        *to_res = to;
    }
    to.size = from_res->size;
    to.data = (char*) malloc(sizeof(char) * (to.size +1));
    if(!to.data)
        return ALLOC_ERROR;
    for(int i = 0; i < to.size; i++)
    {
        to.data[i] = from_res->data[i];
    }
    to.data[to.size] = '\0';
    *to_res = to;
    return OK;
}

//Reading standart input
status_code string_readline(String_ptr res)
{
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    ch = getchar();


    while (ch != '\0' && ch != '\n')
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
                return ALLOC_ERROR;
            char* copyto_str = (char*) realloc(result, max_size);
            if (copyto_str == NULL)
            {
                free(result);
                return ALLOC_ERROR;
            }
            result = copyto_str;
        }

        result[size - 1] = ch;
        size++;
        result[size - 1] = '\0';
        ch = getchar();
    }
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

status_code string_readline_file(String_ptr res, FILE* in)
{
    if(in == NULL)
        return INVALID_PARAMETER;
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    ch = fgetc(in);
    if(ch == EOF)
    {
        free(result);
        res->data = NULL;
        return OK;
    }
    while (ch != '\0' && ch != '\n' && ch != EOF)
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
            {
                free(result);
                return ALLOC_ERROR;
            }
            char* copyto_str = (char*) realloc(result, max_size);
            if (copyto_str == NULL)
            {
                free(result);
                return ALLOC_ERROR;
            }
            result = copyto_str;
        }

        result[size - 1] = ch;
        size++;
        result[size - 1] = '\0';
        ch = fgetc(in);
    }
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

bool is_sep(char c)
{
    return (c == '\n' || c == '\t' || c == ' ');
}
//if bad returns -1
int get_digit(char c)
{
    if(isdigit(c))
    {
        return c - '0';
    }
        
    if (isalpha(c))
    {
        return toupper(c) - 'A' + 10;
    }
    return -1;
}

bool is_digit(char input, int base)
{
    int d = get_digit(input);
    if(d == -1 || d >= base)
        return false;
    return true;
}

status_code string_read_word(String_ptr res)
{
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    while(is_sep(ch))
        ch = getchar();

    while (ch != '\0' && !is_sep(ch))
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
                return ALLOC_ERROR;
            char* copyto_str = (char*) realloc(result, max_size);
            if (copyto_str == NULL)
            {
                free(result);
                return ALLOC_ERROR;
            }
            result = copyto_str;
        }

        result[size - 1] = ch;
        size++;
        result[size - 1] = '\0';
        ch = getchar();
    }
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

status_code string_read_word_file(String_ptr res, FILE* in)
{
    if(in == NULL)
        return INVALID_PARAMETER;
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    while(is_sep(ch))
        ch = fgetc(in);
    if(ch == EOF)
    {
        free(result);
        res->data = NULL;
        return OK;
    }
    while (ch != '\0' && !is_sep(ch) && ch != EOF)
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
                return ALLOC_ERROR;
            char* copyto_str = (char*) realloc(result, max_size);
            if (copyto_str == NULL)
            {
                free(result);
                return ALLOC_ERROR;
            }
            result = copyto_str;
        }

        result[size - 1] = ch;
        size++;
        result[size - 1] = '\0';
        ch = fgetc(in);
    }
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

status_code str_to_uint(String_ptr in, unsigned int* out, int base)
{
    if(in == NULL)
        return INVALID_PARAMETER;
    if(in->data == NULL)
        return INVALID_PARAMETER;
    //Если есть минус
    if(!is_digit((in->data)[0], base))
        return INVALID_PARAMETER;
    errno = 0;
    char* endptr = NULL;
    *out = strtoul(in->data, &endptr, base);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(in->data == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

status_code str_to_int(String_ptr in, int* out, int base)
{
    errno = 0;
    char* endptr = NULL;
    if(in == NULL || in->data == NULL)
        return INVALID_PARAMETER;
    *out = strtol(in->data, &endptr, base);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(in->data == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

status_code str_to_d(String_ptr in, double* out, int base)
{
    errno = 0;
    char* endptr = NULL;
    if(in == NULL || in->data == NULL)
        return INVALID_PARAMETER;
    *out = strtod(in->data, &endptr);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(in->data == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

//[begin;end)
status_code sub_string(String_ptr source, int begin, int end, String_ptr result)
{
    if(source == NULL || source->data == NULL)
        return INVALID_PARAMETER;
    if(begin < 0 || end > source->size)
        return INVALID_PARAMETER;
    char* data = (char*) malloc(sizeof(char) * (end - begin + 1));
    if(!data)
        return ALLOC_ERROR;
    for(int i = begin; i < end; i++)
    {
        data[i - begin] = source->data[i];
    }
    data[end-begin] = '\0';
    status_code error = string_create(data, result);
    free(data);
    return error;
}

int main()
{
    String s1, s2, s3;
    String s4;
    string_create("abc", &s1);
    string_create(NULL, &s2);
    string_create("def", &s3);
    string_create("good", &s4);
    //
    printf("%d\n", string_equal(&s1, &s2));
    printf("%d\n", string_equal(&s2, &s3));
    printf("%d\n", string_equal(&s1, &s3));

    printf("%d\n", string_equal(&s1, &s1));
    printf("%d\n", string_equal(&s2, &s2));
    printf("%d\n", string_equal(&s3, &s3));
    //
    status_code error = OK;
    printf("Concatenating...%s %s\n", s1.data, s2.data);
    error = string_concat(&s1, &s2, &s4);
    printf("Error: %d, result: %s\n", error, s4.data);
    //
    error = OK;
    printf("Concatenating...%s %s\n", s1.data, s3.data);
    error = string_concat(&s1, &s3, &s4);
    printf("Error: %d, result: %s\n", error, s4.data);
    //
    error = OK;
    printf("Copying...%s\n", s1.data);
    error = string_copy(&s1, &s4);
    printf("Error: %d, result: %s\n", error, s4.data);
    //
    error = OK;
    printf("Copying null...%s\n", s4.data);
    error = string_copy(&s4, &s2);
    printf("Error: %d, result: %s\n", error, s2.data);
    //
    String null_str;
    string_create(NULL, &null_str);
    error = OK;
    printf("Copying...%s\n", null_str.data);
    error = string_copy(&null_str, &s4);
    printf("Error: %d, result: %s\n", error, s4.data);
    //
    error = OK;
    string_destroy(&s1);
    string_destroy(&s3);
    string_create(NULL, &s3);
    string_create("abcdefgh", &s1);
    printf("Substr...%d %d %s\n", 0, 3, s1.data);
    error = sub_string(&s1, 0, 3, &s3);
    printf("Error: %d, result: %s\n", error, s3.data);
    //
    error = OK;
    string_destroy(&s1);
    string_destroy(&s3);
    string_create(NULL, &s3);
    string_create("abcdefgh", &s1);
    printf("Substr...%d %d %s\n", 0, 0, s1.data);
    error = sub_string(&s1, 0, 0, &s3);
    printf("Error: %d, result: %s\n", error, s3.data);
    //
    error = OK;
    string_destroy(&s1);
    string_destroy(&s3);
    string_create(NULL, &s3);
    string_create("abcdefgh", &s1);
    printf("Substr...%d %d %s\n", 0, 1, s1.data);
    error = sub_string(&s1, 0, 1, &s3);
    printf("Error: %d, result: %s\n", error, s3.data);
    //
    error = OK;
    string_destroy(&s1);
    string_destroy(&s3);
    string_create(NULL, &s3);
    string_create("abcdefgh", &s1);
    printf("Substr...%d %d %s\n", 0, 100, s1.data);
    error = sub_string(&s1, 0, 100, &s3);
    printf("Error: %d, result: %s\n", error, s3.data);
}