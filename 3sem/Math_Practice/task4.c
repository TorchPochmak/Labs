#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

#define ull unsigned long long int
#define ll long long int

#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)


static const char* usage = "Usage:\n -<flag> <input> or\n /<flag> <input> or\n -n<flag> <input> <output> or\n /n<flag> <input> <output>\n flags: d, i, s, a\n\n";

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

char* get_name(char *PATH){
    char* temp = strrchr(PATH, '/');
    if(temp == NULL)
        return PATH;
    char* name = (char*) malloc(sizeof(char) * strlen(temp) + 5);
    if (name == NULL){
        return NULL;
    }
    name[0] = 'o';
    name[1] = 'u';
    name[2] = 't';
    name[3] = '_';
    if (temp == NULL)
    {
        for (int i = 0; i < strlen(PATH); i++) 
        {
            name[i + 4] = temp[i];
        }
    }
    else {
        for (int i = 0; i < strlen(temp) - 1; i++) {
            name[i + 4] = temp[i + 1];
        }
    }
    return name;
}

enum status_code function_a(FILE* in, FILE* out)
{
    if(in == NULL || out == NULL)
        return INVALID_PARAMETER;

    char c;
    while ((c = fgetc(in)) != EOF)
    {
        if (!isdigit(c)) 
        {
            fprintf(out, "%x ", c);
        }
        else 
        {
            fprintf(out, "%c ", c);
        }
    }
    return OK;
}

enum status_code function_d(FILE* in, FILE* out)
{
    if(in == NULL || out == NULL)
        return INVALID_PARAMETER;

    char c;
    while ((c = fgetc(in)) != EOF) 
    {
        if (!isdigit(c)) 
        {
            fprintf(out, "%c", c);
        }
    }

    return OK;
}

enum status_code function_i(FILE* in, FILE* out)
{
    if(in == NULL || out == NULL)
        return INVALID_PARAMETER;

    int counter = 0;
    char c;
    while ((c = fgetc(in)) != EOF) 
    {
        if (c == '\n') 
        {
            fprintf(out, "%d\n", counter);
            counter = 0;
        }
        else 
        {
            if (isalpha(c)) 
            {
                counter++;
            }
        }
    }
    fprintf(out, "%d \n", counter);
    return OK;
}

enum status_code function_s(FILE* in, FILE* out)
{
    if(in == NULL || out == NULL)
        return INVALID_PARAMETER;

    int counter = 0;
    char c;
    while ((c = fgetc(in)) != EOF) 
    {
        if (c == '\n') 
        {
            fprintf(out, "%d\n", counter);
            counter = 0;
        } 
        else if (isalpha(c) || isdigit(c) || c == ' ' || c == '\t') 
        {
            counter++;
        }
    }
    fprintf(out, "%d\n", counter);
    return OK;
}

enum status_code solve_flag(char flag, FILE* in, FILE* out)
{
    enum status_code code = OK;
    if(in == NULL || out == NULL)
    {
        return INVALID_PARAMETER;
    }
    //-a -d -i -s
    switch (flag)
    {
        case 'a':
        {
            code = function_a(in, out);
            break;
        }
        case 'd':
        {
            code = function_d(in, out);
            break;
        }
        case 'i':
        {
            code = function_i(in, out);
            break;
        }
        case 's':
        {
            code = function_s(in, out);
            break;
        }
        default:
        {
            printf(input_errors[1]);
            code = UNKNOWN_ERROR;
            break;
        }
    }

    return code;
}

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
   argc = 3;
    enum status_code code = OK;
    printf(usage);
    if (argc < 3 || argc > 4)
    {
        printf(input_errors[0]);
        return 1;
    }

    FILE* input;
    FILE* output;

    char in_name[100];
    char* out_name;
    //
    strcpy(in_name, argv[2]);
    //
    if (argc == 3)
    {
        out_name = get_name(argv[2]);
    }
    else
    {
        strcpy(out_name, argv[3]);
    }
    //
    if ((input = fopen(in_name, "r")) == NULL)
	{
		printf("%s", input_errors[3]);
		return 1;
	}
	if ((output = fopen(out_name, "w")) == NULL)
	{
        fclose(input);
		printf("%s", input_errors[3]);
		return 1;
	}
    //
    char flag = '0';
    code = parse_flag(argv[1], &flag);
    if(code != OK)
    {
        fclose(input);
        fclose(output);
        printf("%s", function_base_errors[code]);
        return 1;
    }

    code = solve_flag(flag, input, output);
    if(code != OK)
    {
        printf("%s", function_base_errors[code]);
        fclose(input);
        fclose(output);
        return 1;
    }

    fclose(input);
    fclose(output);
    return 0;
}

