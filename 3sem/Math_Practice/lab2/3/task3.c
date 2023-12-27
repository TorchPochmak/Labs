#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include "my_lib.h"

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

typedef struct
{
    int line;
    int index;
} substr_found;


int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}

bool str_equal(char* a, char* b)
{
    if(a == NULL || b == NULL)
        return false;
    if(strlen(a) != strlen(b))
        return false;
    for(int i = 0; i < strlen(a); i++)
        if(a[i] != b[i])
            return false;
    return true;
}

status_code solve_file(char* substr, int* count, substr_found** result, FILE* file)
{
    if(file == NULL || substr == NULL)
        return INVALID_PARAMETER;
    *count = 0;
    int len = strlen(substr);
    char* buffer = (char*) malloc(sizeof(char) * (len + 1));
    if(buffer == NULL)
        return ALLOC_ERROR;
    buffer[len] = '\0';

    int result_size = 0;
    int max_size = 2;
    *result = (substr_found*) malloc(sizeof(substr_found) * max_size);
    if(*result == NULL)
    {
        free(buffer);
        return ALLOC_ERROR;
    }
    char c = ' ';
    int res;
    int cur_line = 1;
    int cur_index = 1;
    for(int i = 0; i < len; i++)
    {
        c = fgetc(file);
        if(c == EOF)
        {
            free(buffer);
            *count = result_size;
            return OK;
        }
        buffer[i] = c;
    }
    bool first_time = true;
    while (c != EOF)
	{
        if(str_equal(buffer, substr))
        {
            substr_found sub;
            sub.line = cur_line;
            sub.index = cur_index;
            (*result)[result_size] = sub;
            result_size++;
            if(result_size == max_size)
            {
                max_size *= 2;
                substr_found* copy_to = (substr_found*) realloc(*result, sizeof(substr_found) * max_size);
                if(!copy_to)
                {
                    free(buffer);
                    free(*result);
                    return ALLOC_ERROR;
                }
                *result = copy_to;
            }
        }
        if(buffer[0] == '\n')
        {
            cur_index = 0;
            cur_line++;
        }
        cur_index++;
        c = fgetc(file);
        if(c == EOF)
            break;
        for(int i = 1; i < strlen(buffer); i++)
        {
            buffer[i - 1] = buffer[i];
        }
        buffer[strlen(buffer) - 1] = c;
    }
    *count = result_size;
    free(buffer);
    return OK;
}

void free_substrfound(substr_found** arr, int max_index)
{
    if(max_index < 0)
    {
        free(arr);
        arr = NULL;
        return;
    }
    for(int i = 0; i <= max_index; i++)
    {
        free(arr[i]);
        arr[i] = NULL;
    }
    free(arr);
    arr = NULL;
}

status_code solve_files(char* substr, int** result_cnt, substr_found*** subs, int count, ...)
{
    if(substr == NULL || subs == NULL)
        return INVALID_PARAMETER;
    *result_cnt = (int*) calloc(count, sizeof(int));
    *subs = (substr_found**) calloc(count, sizeof(substr_found*));
    if(!(*result_cnt) || !(*subs))
    {
        free(*result_cnt);
        free(*subs);
        return ALLOC_ERROR;
    }
    status_code code = OK;
    va_list argptr;
    va_start(argptr, count);


    for(int i = 0; i < count; i++)
    {
        FILE* f = va_arg(argptr, FILE*);

        substr_found* x;
        if((code = solve_file(substr, &((*result_cnt)[i]), &x, f)) != OK)
        {
            free_substrfound(&x, i - 1);
            return code;
        }
        (*subs)[i] = x;
    }
    va_end(argptr);
    return OK;
}


int main(int argc, char** argv)
{
    
    char* substring = "\\n";
    status_code code = OK;
    FILE* in1 = fopen("input31.txt", "r");
    if(!in1)
    {
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    FILE* in2 = fopen("input32.txt", "r");
    if(!in2)
    {
        fclose(in1);
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    FILE* in3 = fopen("input33.txt", "r");
    if(!in3)
    {
        fclose(in1);
        fclose(in2);
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    int* subs_cnt = 0;
    substr_found** subs = NULL;
    code = solve_files(substring, &subs_cnt, &subs, 3, in1, in2, in3);
    printf("Results:\n");
    for(int i = 0; i < 3; i++)
    {
        bool found = false;
        printf("file number %d\n", i+1);
        printf("%d found\n", subs_cnt[i]);
        for(int j = 0; j < subs_cnt[i]; j++)
        {
            printf("Line: %d, Letter: %d\n", subs[i][j].line, subs[i][j].index);
            found = true;
        }
        if(!found)
            printf("No substrings found in this file!");
        printf("\n");
        
    }
    free_substrfound(subs, 2);
    free(subs_cnt);
    fclose(in1);
    fclose(in2);
    fclose(in3);
    return OK;
}