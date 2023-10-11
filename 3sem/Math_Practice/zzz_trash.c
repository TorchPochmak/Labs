#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_INPUT_SIZE 100

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

enum status_code abs_number(char* number)
{
    if(number == NULL)
        return INVALID_PARAMETER;
    int minuses = 0;
    

}



void fclose_all (int count, ...)
{
    va_list argptr;
    va_start(argptr, count);

    for(int i = 0; i < count; i++)
    {
        int f = va_arg(argptr, int);
        printf("\n%d\n", f);
    }

    va_end(argptr);
}

void free_all(int count, ...)
{
    va_list argptr;
    va_start(argptr, count);

    for(int i = 0; i < count; i++)
    {
        void* obj = va_arg(argptr, void*);
        free(obj);
    }

    va_end(argptr);
}

int main(int argc, char** argv) 
{
    char* g = (char*) malloc(sizeof(char) * 6);
    free_all(2, g, NULL);
   char* dfds = "0";
   printf("GG");
   fclose_all(4, 1,2,3,4);
   getchar();
      return 0;
}