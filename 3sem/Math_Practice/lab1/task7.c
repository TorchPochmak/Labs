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
    FILE_CONTENT_ERROR,
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
    "ERROR: File content error\n"
};

bool is_upper(char c) 
{
    return c >= 'A' && c <= 'Z';
}

bool is_sep(char c)
{
    return (c == '\n' || c == '\t' || c == ' ');
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
    if(*result == NULL)
        return ALLOC_ERROR;
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
            free(*result);
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

enum status_code function_a(FILE* in1, FILE* out)
{
    char single_char[2];
    single_char[1] = '\0';
    single_char[0] = '0';
    bool last_written = false;
    char last_c = ' ';
    enum status_code code = OK;
    char c = ' ';
    int counter = 0;
    while((c = fgetc(in1)) != EOF)
    {
        if(!is_sep(c))
            break;
    }
    if (c == EOF)
        return OK;
    do
    {
        if(!(is_sep(c) || isalpha(c)))
            return FILE_CONTENT_ERROR;
        if(is_sep(last_c) && !is_sep(c))
            counter++;
        if (counter % 10 == 0 && isalpha(c)) 
        {
            if (is_upper(c)) 
            {
                c = c + 32;
            }
            char* result = NULL;

            int num = (int)c;
            char snum[4];
            itoa(num, snum, 10);
            code = convert_base(snum, 10, 4, &result);
            if(code != OK)
            {
                free(result);
                return code;
            }
            fprintf(out, "%s", result);
            free(result);
        }
        else if (counter % 2 == 0 && isalpha(c)) 
        {
            if (is_upper(c)) 
            {
                c = c + 32;
            }
            fprintf(out, "%c", c);
        }
        else if (counter % 5 == 0 && isalpha(c))
        {
            char* result = NULL;
            int num = (int)c;
            char snum[4];
            itoa(num, snum, 10);
            code = convert_base(snum, 10, 8, &result);
            if(code != OK)
            {
                free(result);
                return code;
            }
            fprintf(out, "%s", result);
            free(result);
        }
        if(counter != 0 && (counter % 2 == 0 || counter % 5 == 0) && !is_sep(last_c) && is_sep(c))
            fprintf(out, " ");
        last_c = c;
    }
    while ((c = fgetc(in1)) != EOF);
    return OK;
}

enum status_code function_r(FILE* in1, FILE* in2, FILE* out)
{
    char input[MAX_INPUT_SIZE];
    bool is_first = true;
    bool is_f1_end = false;
    bool is_f2_end = false;
    while (is_f1_end == false && is_f2_end == false) 
    {
        if (is_first)
        {
            if (fscanf(in1, "%s", input) == EOF) 
                is_f1_end = true;
            else
                fprintf(out, "%s ", input);
        }
        else 
        {
            if (fscanf(in2, "%s", input) == EOF) 
                is_f2_end = true;
            else
                fprintf(out, "%s ", input);
        }
        is_first = !is_first;
    }
    //а теперь то, что не считано
    while (fscanf(in1, "%s", input) != EOF) 
    {
        fprintf(out, "%s ", input);
    }

    while (fscanf(in2, "%s", input) != EOF) 
    {
        fprintf(out, "%s ", input);
    }
    return OK;
}

enum status_code solve_flag(char flag, int argc, char** argv)
{
    FILE* in1;
    FILE* in2;
    FILE* out;
    enum status_code code = OK;
    //-r -a
    switch (flag)
    {
        case 'r':
        {
            if(argc != 5)
            {
                printf(input_errors[0]);
                code = INVALID_PARAMETER;
            }
            else
            {
                if ((in1 = fopen(argv[2], "r")) == NULL)
                {
                    printf("%s", input_errors[3]);
                    return INVALID_PARAMETER;
                }
                if ((in2 = fopen(argv[3], "r")) == NULL)
                {
                    fclose(in1);
                    printf("%s", input_errors[3]);
                    return INVALID_PARAMETER;
                }
                if ((out = fopen(argv[4], "w")) == NULL)
                {
                    fclose(in1);
                    fclose(in2);
                    printf("%s", input_errors[3]);
                    return INVALID_PARAMETER;
                }
                code = function_r(in1, in2, out);
                fclose(in1);
                fclose(in2);
                fclose(out);
            }
            break;
        }
        case 'a':
        {
            if(argc != 4)
            {
                printf(input_errors[0]);
                code = INVALID_PARAMETER;
            }
            else
            {
                if ((in1 = fopen(argv[2], "r")) == NULL)
                {
                    printf("%s", input_errors[3]);
                    return INVALID_PARAMETER;
                }
                if ((out = fopen(argv[3], "w")) == NULL)
                {
                    fclose(in1);
                    printf("%s", input_errors[3]);
                    return INVALID_PARAMETER;
                }
                code = function_a(in1, out);
                fclose(in1);
                fclose(out);
            }
            break;
        }
        default:
        {
            printf(input_errors[1]);
            code = INVALID_PARAMETER;
        }
    }
    return code;
}

enum status_code parse_flag(char* in, char* out_char)
{
    if(strlen(in) == 3)
    {
        if((in[0] == '-' || in[0] == '/') && in[1] == 'n')
        {
            *out_char = in[2];
            return OK;
        }
    }
    if(strlen(in) == 2)
    {
        if((in[0] == '-'  || in[0] == '/'))
        {
            *out_char = in[1];
            return OK;
        }
    }
    return INVALID_PARAMETER;
}

int main(int argc, char** argv)
{
    enum status_code code = OK;
    //r - 5 args;
    //a - 4 args;
    // name flag f1 f2 outf
    // name flag f1 outf
    // argc = 4;
    // argv[1] = "-a";
    // argv[2] = "task7_in1.txt";
    // argv[3] = "output.txt";
    // argv[3] = "task7_out.txt";
    if(argc == 5 && !(strcmp(argv[2],argv[3]) && strcmp(argv[2],argv[4]) && strcmp(argv[3],argv[4])))
    {
        printf("%s", function_base_errors[INVALID_PARAMETER]);
        return INVALID_PARAMETER;
    }
    if(argc == 4 && !strcmp(argv[2],argv[3]))
    {
        printf("%s", function_base_errors[INVALID_PARAMETER]);
        return INVALID_PARAMETER;
    }
    if(argc < 4)
    {
        printf("%s", input_errors[0]);
        return 1;
    }
    char flag = ' ';
    code = parse_flag(argv[1], &flag);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        return 1;
    }
    code = solve_flag(flag, argc, argv);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        return 1;
    }
    return 0;
}
