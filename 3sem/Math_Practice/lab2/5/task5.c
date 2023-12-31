#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>

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
    printf("%s", function_base_errors[code]);
    return code;
}

typedef unsigned int uint;

char* dump_memo(void* input, int size) {
    char* res = (char*)malloc(sizeof(char) * size * 9);
    if (!res)
        return NULL;
    int sz = 0;
    char* pt = (char*) input;
    for (int i = 0; i < size; i++) 
    {
        for (int j = 7; j >= 0; j--) 
        {
            char bit = (((*(pt+i)) >> j) & 1) == 0 ? '0' : '1';
            res[sz++] = bit;
        }
        res[sz++] = ' ';
    }
    res[sz] = '\0';
    return res;
}

char int_to_char(int n, bool upper)
{
    if(n < 0 || n >= 36)
        return '\0';
	if (n >= 0 && n < 10)
		return n + '0';
	else
		return n + (upper ? 'A' : 'a') - 10;
}

status_code int_to_str(int number, int base, bool is_upper, char** result)
{
	if(result == NULL)
	{
		return INVALID_PARAMETER;
    }
    int mx_size = 8;
    int size = 0;
    char* res = (char*) calloc(mx_size, sizeof(char));
    if(!res)
        return ALLOC_ERROR;
    if(number == 0)
    {
        res[0] = '0';
        res[1] = '\0';
        *result = res;
        return OK;
    }
	if (base < 2 || base > 36)
	{
		base = 10;
	}
    bool minus = number < 0;
    if(minus)
        res[size++] = '-';
    number = abs(number);
	while (number != 0)
	{
		res[size++] = int_to_char(number % base, is_upper);
		number /= base;
        if(size == mx_size - 1)
        {
            mx_size *= 2;
            char* copy_to = (char*) realloc(res, mx_size * sizeof(char));
            if(!copy_to)
            {
                free(res);
                *result = NULL;
                return ALLOC_ERROR;
            }
            res = copy_to;
        }
	}
	res[size] = '\0';
    int begin = 0;
    int end = size - 1;
    if(minus)
        begin++;
    while(begin > end)
    {
        char t1 = res[begin];
        res[begin] = res[end];
        res[end] = t1;
        begin++;
        end--;
    }
    *result = res;
    return OK;
}

//dynamic
void uint_to_zr(uint num, char** res)
{
    char* result = (char*) calloc(50, sizeof(char));//на uint хватит
    if(!result)
    {
        *res = NULL;
        return;
    }
	uint last = 2971215073; //47th
	uint pred_last = 1836311903;//46th

    int size_ind = 0;
	for (int i = 45; i >= 0; i--)
	{
		if (num >= last)
		{
			num -= last;
			result[i] = '1';
            if(size_ind == 0)
                size_ind = i;
		}
		else
			result[i] = '0';

		uint pred = pred_last;
		pred_last = last - pred_last;
		last = pred;
	}
	result[size_ind + 1] = '1';
	result[size_ind + 2] = '\0';
    *res = result;
}
//dynamic
char* itorom(int input) 
{
    char* result = (char*) calloc(20, sizeof(char));
    if (input < 1 || input > 4000 || result == NULL)
        return NULL;

    int rm_10[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    char* rm_write[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    int i = 0;

    while (input != 0) 
    {
        if (input >= rm_10[i]) 
        {
            strcat(result, rm_write[i]);
            input -= rm_10[i];
        } 
        else
            i++;
    }
    return result;
}

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

char* substr(char* str, int begin_index, int end_index)
{
    if(str == NULL)
        return NULL;
    int len = strlen(str);
    if (begin_index < 0 || end_index > len || begin_index > end_index) 
        return NULL;

    int substr_length = end_index - begin_index + 1;
    char* substring = (char*)malloc((substr_length + 1) * sizeof(char));
    if (!substring) 
        return NULL;
    
    memcpy(substring, &str[begin_index], substr_length);
    substring[substr_length] = '\0'; 
    return substring;
}

int oversprintf(char* buf, char* format, ...) 
{
    buf[0] = '\0';
    int end_ind = 0;
    int begin_ind = 0;
    bool strange_format = true;
    va_list args;
    va_start(args, format);
    int size = strlen(format);
    char* result = NULL;
    int success_read = 0;
    status_code error = OK;
    for (int i = 0; i < size; i++) 
    {
        if (format[i] == '%' && i < size - 1) 
        {
            strange_format = true;
            if (i < size-2 && format[i+1] == 'R' && format[i+2] == 'o') 
            {
                int arg = va_arg(args, int);
                result = itorom(arg);
            }
            else if (i < size-2 && format[i+1] == 'Z' && format[i+2] == 'r') 
            {
                uint arg = va_arg(args, unsigned int);
                uint_to_zr(arg, &result);
            } 
            else if (i < size-2 && format[i+1] == 'C' && format[i+2] == 'v') 
            {
                int arg = va_arg(args, int);
                int base = va_arg(args, int);
                error = int_to_str(arg, base, false, &result);
            }
            else if (i < size-2 && format[i+1] == 'C' && format[i+2] == 'V') 
            {
                int arg = va_arg(args, int);
                int base = va_arg(args, int);
                error = int_to_str(arg, base, true, &result);
            }
            else if (i < size-2 && format[i+1] == 't' && format[i+2] == 'o') 
            {
                int result_num = 0;
                char* arg1 = va_arg(args, char*);
                int arg2 = va_arg(args, int);
                if(arg1[0] == '\0' || (!isdigit(arg1[0]) && !(arg1[0] >= 'a' && arg1[0] <= 'z')))
                    error = INVALID_PARAMETER;
                for(int i = 1; i < strlen(arg1); i++)
                {
                    if(!isdigit(arg1[i]) && !(arg1[i] >= 'a' && arg1[i] <= 'z'))
                        error = INVALID_PARAMETER;
                }
                if(error)
                    result = NULL;
                else 
                {
                    result = (char*) calloc(20, sizeof(char));
                    if(!result)
                        error = ALLOC_ERROR;
                    error = error ? error : str_to_int(&arg1, &result_num, arg2);
                    if(!error)
                        sprintf(result, "%d", result_num);
                }
            }
            else if (i < size-2 && format[i+1] == 'T' && format[i+2] == 'O') 
            {
                int result_num = 0;
                char* arg1 = va_arg(args, char*);
                int arg2 = va_arg(args, int);
                if(arg1[0] == '\0' || (!isdigit(arg1[0]) && !(arg1[0] >= 'A' && arg1[0] <= 'Z')))
                    error = INVALID_PARAMETER;
                for(int i = 1; i < strlen(arg1); i++)
                {
                    if(!isdigit(arg1[i]) && !(arg1[i] >= 'A' && arg1[i] <= 'Z'))
                        error = INVALID_PARAMETER;
                }
                if(error)
                    result = NULL;
                else 
                {
                    result = (char*) calloc(20, sizeof(char));
                    if(!result)
                        error = ALLOC_ERROR;
                    error = error ? error : str_to_int(&arg1, &result_num, arg2);
                    if(!error)
                        sprintf(result, "%d", result_num);
                }
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'i') 
            {
                int n = va_arg(args, int);
                result = dump_memo(&n, sizeof(int));
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'u') 
            {
                uint n = va_arg(args, unsigned int);
                result = dump_memo(&n, sizeof(unsigned int));
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'd')
            {
                double n = va_arg(args, double);
                result = dump_memo(&n, sizeof(double));
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'f') 
            {
                double n = va_arg(args, double);
                result = dump_memo(&n, sizeof(float));
            }
            else 
            {
                end_ind = i+1;
                begin_ind = i;
                bool g = !isalpha(format[end_ind]);

                while (!isalpha(format[end_ind]) && format[end_ind] != '%' && format[end_ind] != '\0') 
                {
                    end_ind++;
                }
				result = substr(format, begin_ind, end_ind);
                strange_format = false;
            }
            if (!strange_format) 
            {
                if(error)
                {
                    free(result);
                    buf[success_read] = '\0';
                    return success_read;
                }
                char buffer[100];
                int vr = vsprintf(buffer, result, args);
                buf[success_read] = '\0';
                success_read = vr < 0 ? success_read : vr + success_read;
                i += strlen(result);
                strcat(buf, buffer);
            }
            else 
            {
                if(error)
                {
                    free(result);
                    buf[success_read] = '\0';
                    return success_read;
                }
                buf[success_read] = '\0';
                if(result == NULL)
                {
                    buf[success_read] = '\0';
                    return success_read;
                }
                success_read += strlen(result);
                strcat(buf, result);
                i += 2;
            }
            free(result);
            result = NULL;
        } 
        else 
        {
            buf[success_read++] = format[i];
        }
    }
    buf[success_read] = '\0';
    va_end(args);
    return success_read;
}

int overfprintf(FILE* file, char* format, ...) 
{
    int end_ind = 0;
    int begin_ind = 0;
    bool strange_format = true;
    va_list args;
    va_start(args, format);
    int size = strlen(format);
    char* result = NULL;
    int success_read = 0;
    status_code error = OK;
    for (int i = 0; i < size; i++) 
    {
        if (format[i] == '%' && i < size - 1) 
        {
            strange_format = true;
            if (i < size-2 && format[i+1] == 'R' && format[i+2] == 'o') 
            {
                int arg = va_arg(args, int);
                result = itorom(arg);
            }
            else if (i < size-2 && format[i+1] == 'Z' && format[i+2] == 'r') 
            {
                uint arg = va_arg(args, unsigned int);
                uint_to_zr(arg, &result);
            } 
            else if (i < size-2 && format[i+1] == 'C' && format[i+2] == 'v') 
            {
                int arg = va_arg(args, int);
                int base = va_arg(args, int);
                error = int_to_str(arg, base, false, &result);
            }
            else if (i < size-2 && format[i+1] == 'C' && format[i+2] == 'V') 
            {
                int arg = va_arg(args, int);
                int base = va_arg(args, int);
                error = int_to_str(arg, base, true, &result);
            }
            else if (i < size-2 && format[i+1] == 't' && format[i+2] == 'o') 
            {
                int result_num = 0;
                char* arg1 = va_arg(args, char*);
                int arg2 = va_arg(args, int);
                if(arg1[0] == '\0' || (!isdigit(arg1[0]) && !(arg1[0] >= 'a' && arg1[0] <= 'z')))
                    error = INVALID_PARAMETER;
                for(int i = 1; i < strlen(arg1); i++)
                {
                    if(!isdigit(arg1[i]) && !(arg1[i] >= 'a' && arg1[i] <= 'z'))
                        error = INVALID_PARAMETER;
                }
                if(error)
                    result = NULL;
                else 
                {
                    result = (char*) calloc(20, sizeof(char));
                    if(!result)
                        error = ALLOC_ERROR;
                    error = error ? error : str_to_int(&arg1, &result_num, arg2);
                    if(!error)
                        sprintf(result, "%d", result_num);
                }
            }
            else if (i < size-2 && format[i+1] == 'T' && format[i+2] == 'O') 
            {
                int result_num = 0;
                char* arg1 = va_arg(args, char*);
                int arg2 = va_arg(args, int);
                if(arg1[0] == '\0' || (!isdigit(arg1[0]) && !(arg1[0] >= 'A' && arg1[0] <= 'Z')))
                    error = INVALID_PARAMETER;
                for(int i = 1; i < strlen(arg1); i++)
                {
                    if(!isdigit(arg1[i]) && !(arg1[i] >= 'A' && arg1[i] <= 'Z'))
                        error = INVALID_PARAMETER;
                }
                if(error)
                    result = NULL;
                else 
                {
                    result = (char*) calloc(20, sizeof(char));
                    if(!result)
                        error = ALLOC_ERROR;
                    error = error ? error : str_to_int(&arg1, &result_num, arg2);
                    if(!error)
                        sprintf(result, "%d", result_num);
                }
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'i') 
            {
                int n = va_arg(args, int);
                result = dump_memo(&n, sizeof(int));
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'u') 
            {
                uint n = va_arg(args, unsigned int);
                result = dump_memo(&n, sizeof(unsigned int));
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'd')
            {
                double n = va_arg(args, double);
                result = dump_memo(&n, sizeof(double));
            }
            else if (i < size-2 && format[i+1] == 'm' && format[i+2] == 'f') 
            {
                double n = va_arg(args, double);
                result = dump_memo(&n, sizeof(float));
            }
            else 
            {
                end_ind = i+1;
                begin_ind = i;
                bool g = !isalpha(format[end_ind]);

                while (!isalpha(format[end_ind]) && format[end_ind] != '%' && format[end_ind] != '\0') 
                {
                    end_ind++;
                }
				result = substr(format, begin_ind, end_ind);
                strange_format = false;
            }
            if (!strange_format) 
            {
                if(error)
                {
                    free(result);
                    return success_read;
                }
                char buffer[100];
                int vr = vsprintf(buffer, result, args);
                success_read = vr < 0 ? success_read : vr + success_read;
                i += strlen(result);
                for(int i = 0; i < strlen(buffer); i++)
                {
                    fputc(buffer[i], file);
                }
            }
            else 
            {
                if(error)
                {
                    free(result);
                    return success_read;
                }
                if(result == NULL)
                {
                    return success_read;
                }
                for(int i = 0; i < strlen(result); i++)
                {
                    fputc(result[i], file);
                }
                success_read += strlen(result);
                i += 2;
            }
            free(result);
            result = NULL;
        } 
        else 
        {
            fputc(format[i], file);
            success_read++;
        }
    }
    va_end(args);
    return success_read;
}


int main()
{
    FILE* f = fopen("out", "w+");
    if(!f)
        return FILE_ERROR;
    char buff[256];
    int count = 10000000;

    printf("\t\tTest 1\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "\\\\%Ro", 3888);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "%Ro", 3888);

    printf("\t\tTest 2\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %Ro", 6);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %Ro", 6);

    printf("\t\tTest 3\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %Roover", 567);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %Roover", 567);

    printf("\t\tTest 4\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %Roover", 100000000);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %Roover", 100000000);

    printf("\t\tTest 5\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %'.2f pupupu %Ro", 1234567.89, 5);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %'.2f pupupu %Ro", 1234567.89, 5);

    printf("\t\tTest 6\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %", 100000000);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %", 100000000);

    printf("\t\tTest 7\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %Cv", 123, 2);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %Cv", 123, 2);

    printf("\t\tTest 8\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %Cv", -123, 16);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %Cv", -123, 16);

    printf("\t\tTest 9\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %CV", 123, 2);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %CV", 123, 2);

    printf("\t\tTest 10\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %CV", -123, 16);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %CV", -123, 16);

    printf("\t\tTest 11\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %to", "1111011", 2);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %to", "1111011", 2);

    printf("\t\tTest 12\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %to", "-7b", 16);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %to", "-7b", 16);

    printf("\t\tTest 13\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %TO", "-7B", -1);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %TO", "-7B", -1);

    printf("\t\tTest 14\n");
    fprintf(f, "\n\tTest 1\n");
    int k;
    count = oversprintf(buff, "over %p over", &k);
    printf("format: 'over %%n over', '&k'\n");
    printf("str: '%s' count:%d int k = %d\n\n", buff, count, k);
    count = overfprintf(f, "over %p over", &k);

    printf("\t\tTest 15\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "%mi", -123);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "%mi", -123);

    printf("\t\tTest 16\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "%mu", 123);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "%mu", 123);

    printf("\t\tTest 17\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "%md", 0.1);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "%md", 0.1);

    printf("\t\tTest 18\n");
   fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "%mf", -1.2);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "%mf", -1.2);

    printf("\t\tTest 19\n");
    fprintf(f, "\n\tTest 1\n");
    count = oversprintf(buff, "over %TO", "-7b", -1);
    printf("str: '%s' count:%d\n\n", buff, count);
    count = overfprintf(f, "over %TO", "-7b", -1);
    
    count = oversprintf(buff, "%");
    return 0;
}
