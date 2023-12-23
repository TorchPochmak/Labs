#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define ull unsigned long long int
#define ll long long int
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

    if (!overflow)
    {
        *res = *res * multiplier;
        return OK;
    }
    else
    {
        return MY_OVERFLOW;
    }
}

//-----------------------

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

enum status_code convert_to_number(char* number, int size, int base, ll* result)
{
    if(base < 2 && base > 36)
        return INVALID_PARAMETER;
    bool overflow = false;

    *result = 0;
    int iterator = 0;
    enum status_code code = OK;
    int power = size - 1;
    ll pw = 0;
    ll elem = 0;
    int digit = 0;

    code = bin_pow(base, power, &pw);
    if(code != OK)
        return code;

    for(int i = 0; i < size; i++)
    {
        digit = get_digit(number[i]);
        if(digit == -1)
            return INVALID_PARAMETER;

        overflow = overflow || mult(pw, digit, &elem);
        overflow = overflow || plus(*result, elem, result);
        pw /= base;
    }
    if(overflow)
        return MY_OVERFLOW;
    return OK;
}

enum status_code solve(FILE* input, FILE* output)
{
    if(input == NULL || output == NULL)
        return INVALID_PARAMETER;
    enum status_code code = OK;
    char cur_ch = '\n';
	
	while (cur_ch != EOF)
	{
        while (is_sep(cur_ch))
		{
			cur_ch = getc(input);
		}

        int max_size = 2;
        int size = 1;
        char* result = (char*) calloc(size + 1, sizeof(char));

        if(result == NULL)
        {
            return ALLOC_ERROR;
        }
        int digit = 0;
        int base = 1;
        bool begin_null = true;

        ll number = 0;
        bool is_num = false;
        int minuses = 0;
        while(cur_ch == '-')
        {
            minuses++;
            cur_ch = getc(input);
        }
        //----------------
        while((digit = get_digit(cur_ch)) != -1)
        {
            is_num = true;
            base = digit + 1 > base ? digit + 1 : base;

            if(size == max_size - 1) 
            {
                max_size *= 2;
                char* copyto_str = (char*) realloc(result, max_size);
                if(copyto_str == NULL)
                {
                    free(result);
                    return ALLOC_ERROR;
                }
                result = copyto_str;
            }

            if(cur_ch != '0')
                begin_null = false;

            if((cur_ch == '0' && !begin_null) || (cur_ch != '0')) 
            {
                result[size - 1] = cur_ch;
                size++;
            }
            cur_ch = getc(input);
        }
        //----------------
        if( (!is_sep(cur_ch) && cur_ch != EOF) || minuses > 1)
        {
            free(result);
            return INVALID_PARAMETER;
        }
        if(is_num && begin_null)
        {
            fprintf(output, "0 2 0\n");
        }
        else{
            result[size - 1] = '\0';
            code = convert_to_number(result, size - 1, base, &number);
            if(code != OK)
                return code;
            if(minuses == 1)
                fprintf(output, "-%s %d -%lld\n", result, base, number);
            else
                fprintf(output, "%s %d %lld\n", result, base, number);
        }
        free(result);
    }

    return OK;
}

int main(int argc, char** argv)
{
    printf(usage);
    if (argc != 3)
    {
        printf(input_errors[1]);
        return 1;
    }
    if(!strcmp(argv[1], argv[2]))
    {
        printf("%s", input_errors[3]);
        return 1;
    }
    FILE* input = NULL;
    FILE* output = NULL;
	
	if ((input = fopen(argv[1], "r")) == NULL)
	{
		printf("%s", input_errors[3]);
		return 1;
	}
	if ((output = fopen(argv[2], "w")) == NULL)
	{
        fclose(input);
		printf("%s", input_errors[3]);
		return 1;
	}
    
    enum status_code code = solve(input, output);

    if(code != OK)
        printf("%s", function_base_errors[code]);

    //closing
    fclose(input);
    fclose(output);
    return 0;
}