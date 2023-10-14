#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define BYTE_SIZE_WRITE 11
#define BYTE_SIZE_READ 4
#define SEEK_SET_SHIFT 3

static const char* usage = "Usage: <file_to_copy> <destination>\n";

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
    printf(usage);

	if (argc != 3)
	{
		printf(function_base_errors[UNKNOWN_INPUT]);
		return UNKNOWN_INPUT;
	}
	
    status_code code = solve(argv[1], argv[2]);
    if(code != OK)
    {
        printf(function_base_errors[code]);
    }
    return code;
}
