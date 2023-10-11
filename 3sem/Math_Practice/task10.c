#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>

#define MAX_INPUT_SIZE 1024

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
    "ERROR: Alloc Error\n",
    "ERROR: Specific for function\n"
};

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


bool is_sep(char c)
{
    return (c == '\n' || c == '\t' || c == ' ');
}

bool is_stop(char* input)
{
    if(input == NULL || strlen(input) != 4)
        return false;
    char* str = "Stop";
    for(int i = 0; i < 5; i++)
    {
        if(str[i] != input[i])
        {
            return false;
        }
    }
    return true;
}

bool check_number_abs_and_base(char** number, int base)
{
    if (*number == NULL || number == NULL)
        return false;
    int len = strlen(*number);
    int digit = 0;
    for(int i = 0; i < len; i++)
    {
        if((digit = get_digit((*number)[i])) == -1)
            return false;
        if(digit >= base)
            return false;
    }
    return true;
}

enum status_code abs_greater(char** num1, char** num2, char** result, bool has_minus1, bool has_minus2, bool *has_minus_res, int base)
{
    if (!(check_number_abs_and_base(num1, base) && check_number_abs_and_base(num2, base)))
        return INVALID_PARAMETER;

    int len1 = strlen(*num1);
    int len2 = strlen(*num2);
    int digit1 = 0, digit2 = 0;

    if(len1 > len2)
    {
        *result = (char*) malloc(sizeof(char) * len1);
        if(result == NULL)
            return ALLOC_ERROR;
        strcpy(*result, *num1);
        *has_minus_res = has_minus1;
    }
    else if (len1 < len2)
    {
        *result = (char*) malloc(sizeof(char) * len2);
        if(result == NULL)
            return ALLOC_ERROR;
        strcpy(*result, *num2);
        *has_minus_res = has_minus2;
    }
    else
    {
        *result = (char*) malloc(sizeof(char) * len2);
        if(result == NULL)
            return ALLOC_ERROR;
        bool first_greater = true;
        for (int i = 0; i < len1; i++)
        {
            digit1 = get_digit((*num1)[i]);
            digit2 = get_digit((*num2)[i]);
            if (digit1 < digit2)
            {
                first_greater = false;
                break;
            }
        }
        if (first_greater)
        {
            strcpy(*result, *num1);
            *has_minus_res = has_minus1;
        }
        else
        {
            strcpy(*result, *num2);
            *has_minus_res = has_minus2;
        }
    }
    return OK;
}


enum status_code abs_number(char** num, char** result, bool* has_minus)
{
    char* number = *num;
    char cur_ch = ' ';
    if(number == NULL)
        return INVALID_PARAMETER;

    int max_size = 2;
    int size = 1;

    *result = (char*) calloc(size + 1, sizeof(char));
    if(*result == NULL)
    {
        return ALLOC_ERROR;
    }
    
    int digit = 0;
    bool begin_null = true;

    bool is_num = false;
    int minuses = 0;
    
    int iter = 0;
    cur_ch = number[iter];
    while(cur_ch == '-')
    {
        minuses++;
        iter++;
        cur_ch = number[iter];
    }
    //----------------
    while((digit = get_digit(cur_ch)) != -1)
    {
        is_num = true;

        if(size == max_size - 1) 
        {
            max_size *= 2;
            char* copyto_str = (char*) realloc(*result, max_size);
            if(copyto_str == NULL)
            {
                free(*result);
                return ALLOC_ERROR;
            }
            *result = copyto_str;
        }

        if(cur_ch != '0')
            begin_null = false;

        if((cur_ch == '0' && !begin_null) || (cur_ch != '0')) 
        {
            (*result)[size - 1] = cur_ch;
            size++;
        }
        iter++;
        cur_ch = number[iter];
    }
    //----------------
    if( (!is_sep(cur_ch) && cur_ch != EOF && cur_ch != '\0') || minuses > 1)
    {
        return INVALID_PARAMETER;
    }
    if(is_num && begin_null)
    {
        (*result)[0] = '0';
        (*result)[1] = '\0';
    }
    else
    {
        (*result)[size - 1] = '\0';
    }

    if (minuses == 1)
        *has_minus = true;
    else
        *has_minus = false;

    return OK;
}


enum status_code str_to_int(char** in, int* out, int base)
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

enum status_code to_upper(char** s)
{
    if(s == NULL || *s == NULL)
        return INVALID_PARAMETER;
    int len = strlen(*s);
    for(int i = 0; i < len; i++)
    {
        (*s)[i] = toupper((*s)[i]);
    }
    return OK;
}

enum status_code read_string(char** res)
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

enum status_code convert_base(char* input, int inputBase, int outputBase, char** result) 
{
    enum status_code code = OK;
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

int main(int argc, char** argv) 
{
    bool has_maximum_minus = false;
    bool has_current_minus = false;

    if(argc != 1)
    {
        printf("Wrong count of parameters");
        return 1;
    }
    char* maximum;
    enum status_code code = OK;
    int base = 0;
    char* buff = (char*) calloc(MAX_INPUT_SIZE, sizeof(char));


    if(buff == NULL)
    {
        code = ALLOC_ERROR;
        printf("%s", function_base_errors[code]);

        return 1;
    }
    printf("Enter base of numbers: ");
    //------------------------
    code = read_string(&buff);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        free(buff);
        return 1;
    }
    code = str_to_int(&buff, &base, 10);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        free(buff);
        return 1;
    }
    //-----------------------
    char* current_number = NULL;
    char* max_number = NULL;
    //----------
    if(base < 2 || base > 36)
    {
        printf("Incorrect base");
        free(buff);
        return 1;
    }
    if(is_stop(buff))
    {
        printf("No numbers enetered");
        free(buff);
        return 1;
    }
    while(true)
    {
        //----------
        code = read_string(&buff);

        if (is_stop(buff))
            break;

        code = to_upper(&buff);
        if(code != OK)
        {
            printf("%s", function_base_errors[code]);
            free(buff);
            free(current_number);
            free(max_number);
            return 1;
        }
        //
        code = abs_number(&buff, &current_number, &has_current_minus);
        if (code != OK)
        {
            printf("%s", function_base_errors[code]);
            free(buff);
            free(current_number);
            free(max_number);
            return 1;
        }
        if (check_number_abs_and_base(&current_number, base))
        {
            if(max_number == NULL)
            {
                max_number = current_number;
                has_maximum_minus = has_current_minus;
            }
            else
            {
                char* result;
                code = abs_greater(&current_number, &max_number, &result, has_current_minus, has_maximum_minus, &has_maximum_minus, base);
                free(current_number);
                free(max_number);
                if(code != OK)
                {
                    printf("%s", function_base_errors[code]);
                    free(result);
                    free(buff);
                    return 1;
                }

                max_number = result;
            }
        }
        else
        {
            printf("Incorrect number\n");
            free(buff);
            free(current_number);
            free(max_number);
            return 1;
        }
    }
    char* result = NULL;
    for(int i = 9; i <= 36; i += 9)
    {
        code = convert_base(max_number, base, i, &result);
        if(code != OK)
        {
            printf("%s", function_base_errors[code]);
            free(buff);
            free(max_number);
            free(result);
            return 1;
        }
        if(has_maximum_minus)
            printf("Maximum number in base %d is -%s\n", i, result);
        else
            printf("Maximum number in base %d is %s\n", i, result);
    }
    free(buff);
    free(max_number);
    free(result);
    return 0;
}