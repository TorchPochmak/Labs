#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


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
    PROCESS_CREATE_ERROR,
} status_code;

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Input error\n",
    "ERROR: Unknown...\n",
    "ERROR: Process was not created\n"
};

int show_error(status_code code)
{
    printf("%s", function_base_errors[code]);
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

status_code solve_file(char* substr, int* count, FILE* file)
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
    char c = ' ';
    int res;
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
            result_size++;
        }
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

int is_sep(char c)
{
    return (c == '\n' || c == '\r' || c == ' ' || c == '\t');
}

int main(int argc, char** argv)
{
    //argc = 2;
    //argv[1] = "t43";
    if(argc != 2)
    {
        show_error(INPUT_ERROR);
        return INPUT_ERROR;
    }
    FILE* file = fopen(argv[1], "r");
    if(!file)
    {
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    char* substr = "\n";
    char c = ' ';
    int max_size = 2;
    int size = 0;
    char* name = (char*) calloc(max_size, sizeof(char));
    if(!name)
    {
        fclose(file);
        return ALLOC_ERROR;
    } 
    status_code error = OK;
    pid_t pid;

    while(c != EOF)
    {
        c = fgetc(file);
        if(is_sep(c) || c == EOF)
        {
            name[size] = '\0';
            pid = fork();
            if(pid == 0)
            {
                int res = 0;
                FILE* f = fopen(name, "r");
                if(!f)
                {
                    free(name);
                    name = NULL;
                    show_error(FILE_ERROR);
                    return FILE_ERROR;
                }
                error = solve_file(substr, &res, f);
                if(error)
                {
                    free(name);
                    name = NULL;
                    show_error(error);
                    return error;
                }
                if(res > 0)
                {
                    printf("%s\n", name);
                }
                int ret = res > 0 ? 1 : 0;
                //printf("%d found, \n", ret);
                _Exit(ret);
            }
            else if(pid == -1)
            {
                free(name);
                name = NULL;
                show_error(PROCESS_CREATE_ERROR);
                return PROCESS_CREATE_ERROR;
            }
            else{
                free(name);
                name = NULL;
                max_size = 2;
                size = 0;
                name = (char*) calloc(max_size, sizeof(char));
                if(!name)
                {
                    fclose(file);
                    show_error(ALLOC_ERROR);
                    return ALLOC_ERROR;
                } 
            }
        }
        if(size == max_size)
        {
            max_size *= 2;
            char* copy_to = (char*) realloc(name, max_size * sizeof(char));
            if(!copy_to)
            {
                free(name);
                name = NULL;
                show_error(ALLOC_ERROR);
                return ALLOC_ERROR;
            }
            name = copy_to;
        }
        if(!is_sep(c))
            name[size++] = c;
    }

    free(name);
    name = NULL;
    fclose(file);
    int exit_status = 0;
    int result_files = 0;
    pid_t cpid;
    while((cpid = wait(&exit_status)) > 0)
    {
        if(WIFEXITED(exit_status))
            result_files += WEXITSTATUS(exit_status);
    }
    //printf("%d\n", result_files);
    if(result_files == 0)
    {
        printf("Nothing was found in files\n");
    }
    return OK;
}