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
    "ERROR: Specific for function...\n"
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



status_code str_to_int(char** in, int32_t* out, int base)
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



status_code convert_hex_to_mask(char* hex, int32_t mask)
{
    status_code code = OK;
    if(strlen(hex) != 4)
        return INVALID_PARAMETER;

    for (int i = 0; i < 4; i++)
    {
        int digit = get_digit(hex[i]);
        if(digit >= 16 || digit == -1)
        {
            return INVALID_PARAMETER;
        }
    }
    //validation done
    code = str_to_int(&hex, &mask, 16);
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
    return OK;
}

status_code xor32sum(char* filename, int* result)
{
    FILE* file;
    if ((file = fopen(filename, "r")) == NULL)
    {
        return FILE_ERROR;
    }

    char buff[4];
    int count = 0;
    *result = 0;
    while ((count = fread(buff, sizeof(char), 4, file)) != 0)
	{


    }

}

status_code solve_flag(int argc, char** argv)
{
    status_code code = OK;

    if(argc < 3 || argc > 4)
        return UNKNOWN_INPUT;

    switch(argv[1])
    {
        case "xor8":
        {
            if(argc != 3)
                return UNKNOWN_INPUT;



            //TODO func1
            break;
        }
        case "xor32":
        {
            if(argc != 3)
                return UNKNOWN_INPUT;
            

            //TODO func2
            break;
        }
        case "mask":
        {
            if(argc != 4)
                return UNKNOWN_INPUT;
            int32_t mask = 0;
            code = convert_hex_to_mask(argv[3], &mask);

            if(code != OK)
                return code;

            //TODO func3


            break;
        }
    }
    return OK;
}



status_code solve(char* filename_in, char* filename_out)
{
    FILE* in = NULL;
    FILE* out = NULL;
     
    if ((in = fopen(filename_in, "r")) == NULL)
    {
        return FILE_ERROR;
    }
    if ((out = fopen(filename_out, "w")) == NULL)
    {
        fclose(in);
        return FILE_ERROR;
    }

    char c;
    while ((c = (char)fgetc(in)) != EOF){
        fputc(c, out);
    }

    fclose(in);
    fclose(out);

    return OK;
}

int main(int argc, char** argv)
{
    status_code code = OK;
    printf(usage);
    code = solve_flag(argc, argv);
	
    status_code code = solve(argv[1], argv[2]);
    if(code != OK)
    {
        printf(function_base_errors[code]);
    }
    return code;
}
