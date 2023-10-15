#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_BUFF_SIZE 1024

#define BYTE_SIZE_WRITE 11
#define BYTE_SIZE_READ 4
#define SEEK_SET_SHIFT 3

static const char* usage = "Usage: <command> <file> [<hex> - for command mask]\n";

typedef enum
{
    OK = 0,
    INVALID_PARAMETER,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    ALLOC_ERROR,
    FILE_ERROR,
    UNKNOWN_INPUT,
    UNKNOWN_ERROR,
    RESERVED
} status_code;


static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: OVERFLOW\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Unknown input\n",
    "ERROR: Unknown...\n",
    "ERROR: Not enough numbers to xor\n"
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

status_code convert_hex_to_mask(char* hex, int* mask)
{
    status_code code = OK;
    
    //validation done
    code = str_to_int(&hex, mask, 16);
    return code;
}

status_code xor8sum(char* filename, int* result)
{
    FILE* file;
    if ((file = fopen(filename, "r")) == NULL)
    {
        return FILE_ERROR;
    }
    char c; 
    *result = 0;

    while((c = fgetc(file)) != EOF)
    {
        *result ^= c;
    }
    fclose(file);
    return OK;
}

status_code xor32sum(char* filename, int* result)
{
    FILE* file;
    if ((file = fopen(filename, "r")) == NULL)
    {
        return FILE_ERROR;
    }

    unsigned char buff[4];
    int count = 0;
    *result = 0;
    int counter = 0;
    while ((count = fread(&buff, sizeof(char), 4, file)) != 0)
	{
        int tmp = 0;
        for (int i = 0; i < count; i++) {
            int shift_bytes = 3-i;
            tmp += buff[i] << (shift_bytes * 8);
        }
        *result ^= tmp;
        counter++;
    }
    if(counter == 1)
    {
        return RESERVED;
    }
    fclose(file);
    return OK;
}

status_code get_count_max(char* filename, int mask, int* result)
{
    *result = 0;
    FILE* file;
    if ((file = fopen(filename, "r")) == NULL)
    {
        return FILE_ERROR;
    }
    int tmp = 0;

    while (fread(&tmp, sizeof(int), 1, file) == 1)
    {
        if(tmp & mask == mask)
        {
            if(*result == INT_MAX)
            {
                fclose(file);
                return MY_OVERFLOW;
            }
            (*result)++;
        }

    }
    fclose(file);
    return OK;
}

status_code solve_flag(int argc, char** argv)
{
    status_code code = OK;

    if(argc < 3 || argc > 4)
        return UNKNOWN_INPUT;

    if(strcmp(argv[2],"xor8") == 0)
    {
        if(argc != 3)
            return UNKNOWN_INPUT;
        int result = 0;
        code = xor8sum(argv[1],&result);
        if(code != OK)
            return code;
        printf("xor8 result: %d", result);
    }
    else if(strcmp(argv[2],"xor32") == 0)
    {
        if(argc != 3)
            return UNKNOWN_INPUT;
        int result = 0;
        code = xor32sum(argv[1],&result);
        if(code != OK)
            return code;
        printf("xor32 result: %d", result);
    }
    else if(strcmp(argv[2],"mask") == 0)
    {
        if(argc != 4)
            return UNKNOWN_INPUT;
        int mask = 0;
        code = convert_hex_to_mask(argv[3], &mask);

        if(code != OK)
            return code;
        int result = 0;
        code = get_count_max(argv[1], mask, &result);
        if(code != OK)
            return code;
        printf("mask result: %d", result);
    }
    else
    {
        return UNKNOWN_ERROR;
    }
    return code;
}


int main(int argc, char** argv)
{
    // argc = 4;
    // argv[1] = "input";
    // argv[2] = "mask";
    // argv[3] = "31313131";

    status_code code = OK;
    printf(usage);
    code = solve_flag(argc, argv);

    if(code != OK)
    {
        printf(function_base_errors[code]);
    }
    return code;
}
