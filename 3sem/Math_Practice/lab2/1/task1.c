#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>
//#include "my_lib.h"

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
    RESERVED,
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
    "ERROR: Specific...\n"
};

int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
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

status_code str_to_uint(char** in, unsigned int* out, int base)
{
    if(in == NULL || *in == NULL)
        return INVALID_PARAMETER;

    //Если есть минус
    if(!is_digit((*in)[0], base))
        return INVALID_PARAMETER;
    errno = 0;
    char* endptr = NULL;
    *out = strtoul(*in, &endptr, base);
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

int my_strlen(char* str)
{
    if(str == NULL)
        return 0;
    int i = 0;
    while (str[i] != '\0')
        i++;
    return i;
}

status_code validate(int argc, char** argv)
{
    if(argc < 3)
        return INPUT_ERROR;
    if(my_strlen(argv[1]) != 2)
        return INPUT_ERROR;
    //
    char flag = argv[1][1];
    char flag_ch = argv[1][0];
    //
    if(flag_ch != '-')
        return INPUT_ERROR;
    if((flag == 'l' || flag == 'r' || flag == 'u' || flag == 'n') && argc == 3)
        return OK;
    if(flag == 'c' && argc >= 4)
    {
        status_code code = OK;
        unsigned int result = 0;
        
        code = str_to_uint(&argv[2], &result, 10);
        if(code != OK)
            return INVALID_PARAMETER;
        return OK;
    }
    return INVALID_PARAMETER;
}

status_code reverse_str(char* str, char** result)
{
    int len = my_strlen(str);
    *result = (char*) calloc(len + 1, sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len; i++)
    {
        (*result)[len - i - 1] = str[i];
    }
    (*result)[len] = '\0';
    return OK;
}

status_code solve_u(char* str, char** result)
{
    int len = my_strlen(str);
    (*result) = (char*) calloc(len + 1, sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len; i++)
    {
        if(i % 2 == 1)
            (*result)[i] = toupper(str[i]);
        else
            (*result)[i] = str[i];
    }
    (*result)[len] = '\0';
    return OK;
}

status_code solve_n(char* str, char** result)
{
    int len = my_strlen(str);
    
    *result = (char*) calloc(len + 1, sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    int digit_count = 0, alpha_count = 0, other_count = 0;
    int digit_ind = 0, alpha_ind = 0, other_ind = 0;
    for(int i = 0; i < my_strlen(str); i++)
    {
        if(isdigit(str[i]))
            digit_count++;
        else if(isalpha(str[i]))
            alpha_count++;
        else
            other_count++;
    }
    alpha_ind = digit_count;
    other_ind = digit_count + alpha_count;
    for(int i = 0; i < my_strlen(str); i++)
    {
        if(isdigit(str[i]))
        {
            (*result)[digit_ind] = str[i];
            digit_ind++;
        }
        else if(isalpha(str[i]))
        {
            (*result)[alpha_ind] = str[i];
            alpha_ind++;
        }
        else
        {   
            (*result)[other_ind] = str[i];
            other_ind++;
        }
    }
    (*result)[len] = '\0';
    return OK;
}

//returns an array of strings
status_code solve_c(int argc, char** argv, char** result)
{
    status_code code = OK;

    unsigned int seed = 0;
    
    if((code = str_to_uint(&argv[2], &seed, 10)) != OK)
        return code;
    srand(seed);
    
    int count = argc - 3;
    //Заполнен false
    bool* done = (bool*) calloc(count, sizeof(int));
    if(done == NULL)
        return ALLOC_ERROR;
    int all_count = 0;
    for(int i = 3; i < argc; i++)
    {
        all_count += my_strlen(argv[i]);
    }    

    *result = (char*) calloc(all_count + count, sizeof(char));
    if(result == NULL || *result == NULL)
        return ALLOC_ERROR;
    int concatenated = 0;
    int result_index = 0;
    while(concatenated != count)
    {
        int random = rand() % count;
        if(!done[random])
        {
            done[random] = true;
            int argv_index = random == 0 ? 3 : random + 3;
            int len = my_strlen(argv[argv_index]);
            for(int i = 0; i < len; i++)
            {
                (*result)[result_index] = argv[argv_index][i];
                result_index++;
            }
            concatenated++;
        }
    }
    (*result)[result_index] = '\0';
    return OK;
}

status_code solve_flags(int argc, char** argv)
{
    status_code code = OK;
    char flag = argv[1][1];
    switch(flag)
    {
        case 'l':
        {
            printf("Length of input string is %d", my_strlen(argv[2]));
            break;
        }
        case 'r':
        {
            char* result = NULL;
            if((code = reverse_str(argv[2], &result)) != OK)
                return code;
            printf("Reversed input string is: %s", result);
            free(result);
            break;
        }
        case 'u':
        {
            char* result = NULL;
            if((code = solve_u(argv[2], &result)) != OK)
                return code;
            printf("ToUpper odd positions in input str is: %s", result);
            free(result);
            break;
        }
        case 'n':
        {
            char* result = NULL;
            if((code = solve_n(argv[2], &result)) != OK)
                return code;
            printf("Sorted input string (digits, alpha, other) is: %s", result);
            free(result);
            break;
        }
        case 'c':
        {
            char* result;
            if((code = solve_c(argc, argv, &result)) != OK)
                return code;
            printf("Concatenated strings, using seed: %s", result);
            free(result);
            break;
        }
        default:
        {
            return UNKNOWN_ERROR;
        }
    }
    return code;
}

int main(int argc, char** argv)
{
    status_code code = OK;
    if((code = validate(argc, argv)) != OK)
        return show_error(code);
    
    if((code = solve_flags(argc, argv)) != OK)
        return show_error(code);
    
    return OK;
}