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

static const char* usage = "Usage: <file>\n";

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

status_code solve(char* filename)
{
    FILE* file;
    //-----
	if ((file = fopen(filename, "w")) == NULL)
		return FILE_ERROR;
	
	unsigned char bytes_to_write[BYTE_SIZE_WRITE] = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5 };
	
	if (fwrite(bytes_to_write, sizeof(char), BYTE_SIZE_WRITE, file) != BYTE_SIZE_WRITE)
	{
		fclose(file);
		return FILE_ERROR;
	}
	fclose(file);
    //-----
	if ((file = fopen(filename, "rb")) == NULL)
		return FILE_ERROR;

    char c;
    //before
    //typedef struct _iobuf
    // {
    //     char*   _ptr;
    //     int _cnt;
    //     char*   _base;
    //     int _flag;
    //     int _file;
    //     int _charbuf;
    //     int _bufsiz;
    //     char*   _tmpfname;
    // } FILE;

    //now
    // typedef struct _iobuf
    // {
    //     void* _Placeholder;
    // } FILE;
	while (fread(&c, sizeof(char), 1, file))
	{
		printf("%x %p\n", c, file->_Placeholder);
	}
	fclose(file);
    //-----
	
	if ((file = fopen(filename, "rb")) == NULL)
		return FILE_ERROR;
	
	fseek(file, SEEK_SET_SHIFT, SEEK_SET);
	unsigned char bytes_to_read[BYTE_SIZE_READ];
	
	if (fread(bytes_to_read, sizeof(char), BYTE_SIZE_READ, file) != BYTE_SIZE_READ)
	{
		fclose(file);
		return FILE_ERROR;
	}
	
	for (int i = 0; i < BYTE_SIZE_READ; i++)
	{
		printf("%x ", bytes_to_read[i]);
	}
	fclose(file);
    return OK;
}

int main(int argc, char** argv)
{
    printf(usage);

	if (argc != 2)
	{
		printf(function_base_errors[UNKNOWN_INPUT]);
		return UNKNOWN_INPUT;
	}
	
    status_code code = solve(argv[1]);
    if(code != OK)
    {
        printf(function_base_errors[code]);
    }
    return code;
}
