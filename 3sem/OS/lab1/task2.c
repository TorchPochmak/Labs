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

status_code get_data()
{




}

int main(int argc, char** argv)
{




}