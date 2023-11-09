#include "my_lib.h"

int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}

//Some doubles...

bool equal_d(double a, double b, double eps)
{
    return fabsl(a-b) < eps;
}

// Converting...

status_code str_to_int(char** in, int* out, int base)
{
    errno = 0;
    char* endptr = NULL;
    *out = strtol(*in, &endptr, base);
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

status_code str_to_d(char** in, double* out)
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

status_code convert_base(char* input, int inputBase, int outputBase, char** result) 
{
    status_code code = OK;
    int decimal;
    code = str_to_int(&input, &decimal, inputBase);

    if(code != OK)
        return code;
    if(decimal == 0)
    {
        *result = "0";
        return OK;
    }
    
    int max_size = 32;

    *result = (char*) malloc(sizeof(char) * max_size);
    int index = 0;

    while (decimal > 0) {
        if (index == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
                return ALLOC_ERROR;
            char* copyto_str = (char*) realloc(*result, max_size);
            if (copyto_str == NULL)
            {
                free(*result);
                return ALLOC_ERROR;
            }
            *result = copyto_str;
        }
        int remainder = decimal % outputBase;
        decimal /= outputBase;

        (*result)[index++] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'A');
    }

    // Переворачиваем строку, чтобы получить правильный порядок символов
    for (int i = 0, j = index - 1; i < j; i++, j--) {
        char temp = (*result)[i];
        (*result)[i] = (*result)[j];
        (*result)[j] = temp;
    }
    (*result)[index] = '\0';
    return OK;
}

//Reading standart input

status_code read_string(char** res)
{
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
    *res = result;
    return OK;
}

status_code my_strcat(char* in1, char* in2, char** out)
{
    if(in1 == NULL || in2 == NULL)
        return INVALID_PARAMETER;
    int len1 = strlen(in1);
    int len2 = strlen(in2);

    *out = (char*) calloc(len1 + len2 + 1, sizeof(char));
    if(out == NULL || *out == NULL)
        return ALLOC_ERROR;

    for(int i = 0; i < len1; i++)
        (*out)[i] = in1[i];

    for(int j = 0; j < len2; j++)
        (*out)[j] = in2[j];

    (*out)[len1 + len2] = '\0';
    return OK;
}

//Just some more

void fclose_all (int count, ...)
{
    va_list argptr;
    va_start(argptr, count);

    for (int i = 0; i < count; i++)
    {
        FILE* f = va_arg(argptr, FILE*);
        fclose(f);
    }

    va_end(argptr);
}

void free_all(int count, ...)
{
    va_list argptr;
    va_start(argptr, count);

    for (int i = 0; i < count; i++)
    {
        void* obj = va_arg(argptr, void*);
        free(obj);
    }

    va_end(argptr);
}

// Printing arrs

void print_double_arr(double* arr, int count, char* sep)
{
    if(sep == NULL)
        sep = " ";
    for(int i = 0; i < count - 1; i++)
    {
        printf("%lf%s", arr[i], sep);
    }
    printf("%lf", arr[count - 1]);
}

void print_int_arr(int* arr, int count, char* sep)
{
    if(sep == NULL)
        sep = " ";
    for(int i = 0; i < count - 1; i++)
    {
        printf("%d%s", arr[i], sep);
    }
    printf("%d", arr[count - 1]);
}