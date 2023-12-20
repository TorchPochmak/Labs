#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
//#include "my_lib.h"

//оценка в пределах {1,2,3,4,5}
#define EXAMS_COUNT 5

const double EPS = 1e-10;

typedef struct 
{
    unsigned int id;
    char surname[50];
    char name[50];
    double salary;
} Employee;

//-----------------------------------------------------------------------------------------
//MY_LIB
#define MAX_INPUT_SIZE 1024 //Buffer size
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
    RESERVED
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
    "ERROR: Reserved...\n"
};

void fclose_all (int count, ...)
{
    va_list argptr;
    va_start(argptr, count);

    for (int i = 0; i < count; i++)
    {
        FILE* f = va_arg(argptr, FILE*);
        fclose(f);
    }

    va_end(argptr);
}

void free_all(int count, ...)
{
    va_list argptr;
    va_start(argptr, count);

    for (int i = 0; i < count; i++)
    {
        void* obj = va_arg(argptr, void*);
        free(obj);
    }

    va_end(argptr);
}

int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}

status_code read_string(char** res)
{
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    ch = getchar();


    while (ch != '\0' && ch != '\n')
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
                return ALLOC_ERROR;
            char* copyto_str = (char*) realloc(result, max_size);
            if (copyto_str == NULL)
            {
                free(result);
                return ALLOC_ERROR;
            }
            result = copyto_str;
        }

        result[size - 1] = ch;
        size++;
        result[size - 1] = '\0';
        ch = getchar();
    }
    *res = result;
    return OK;
}

//--------------------------------------------------------------------------------
//Just some more

status_code check_flags(int argc, char** argv)
{
    if(argc != 4)
        return INPUT_ERROR;
    bool ok = argv[2][0] == '-' || argv[2][0] == '/';
    ok = ok && argv[2][1] == 'a' || argv[2][1] == 'd';
    ok = ok && (strlen(argv[2]) == 2);
    if(ok)
        return OK;
    else 
        return INPUT_ERROR;
}

bool check_employee(Employee* empl)
{
    bool ok = true;
    if(empl == NULL || strlen(empl->name) == 0 || strlen(empl->surname) == 0 || empl->salary < 0)
        return false;
    for(int i = 0; i < strlen(empl->name); i++)
    {
        ok = ok && isalpha(empl->name[i]);
    }
    for(int i = 0; i < strlen(empl->surname); i++)
    {
        ok = ok && isalpha(empl->surname[i]);
    }
    return ok;
}

bool employees_list_valid(Employee* list, int empl_count)
{
    if(list == NULL || empl_count < 0)
        return INVALID_PARAMETER;
    for(int i = 0; i < empl_count; i++)
    {
        if(!check_employee(&list[i]))
            return false;
    }
    return true;
}

int cmp_asc(const void* av, const void* bv)
{
    int res = 0;
    Employee* a = (Employee*) av;
    Employee* b = (Employee*) bv;
    if(fabs(a->salary - b->salary) < 0)
    {
        int surname_cmp = strcmp(a->surname, b->surname);
        if(surname_cmp == 0)
        {
            int name_cmp = strcmp(a->name, b->name);
            if(name_cmp == 0)
            {
                return a->salary - b->salary;
            }   
            else
                res = name_cmp > 0 ? 1 : -1;
        }
        else
            res = surname_cmp > 0 ? 1 : -1;
    }
    else
        res = a->salary - b->salary > 0 ? 1 : -1;
    return res;
}

int cmp_dsc(const void* av, const void* bv)
{
    return cmp_asc(bv, av);
}

//using student_list_valid
status_code employees_parse(FILE* in, Employee** result, int* result_count)
{
    status_code code = OK;
    if(in == NULL || result == NULL)
        return INVALID_PARAMETER;
    //
    *result_count = 0;
    int result_maxsz = 1;
    //
    Employee* empls = (Employee*) malloc(result_maxsz * sizeof(Employee));
    if(empls == NULL)
        return ALLOC_ERROR;
    Employee* last_st = &(empls[0]);
    
    while(!feof(in))
    {
        int g = last_st->id;
        if(fscanf(in, "%u %50[^ ] %50[^ ] %lf\n",
            &(last_st->id), (last_st->surname), &(last_st->name), &(last_st->salary)) != 4)
        {
            code = INPUT_ERROR;
            free(empls);
            return code;
        }

        (*result_count)++;
        if(*result_count == result_maxsz)
        {
            result_maxsz *= 2;
            Employee* copyto = (Employee*) realloc(empls, sizeof(Employee) * result_maxsz);
            if(copyto == NULL)
            {
                free(empls);
                code = ALLOC_ERROR;
                return code;
            }
            else
            {
                empls = copyto;
            }
        }
        last_st = &(empls[*result_count]);
    }
    if(!employees_list_valid(empls, *result_count))
    {
        free(empls);
        return INPUT_ERROR;
    }
    *result = empls;
    return code;
}

void print_empllist(FILE* file, Employee* list, int cnt)
{
    if(list == NULL || !employees_list_valid(list, cnt) || file == NULL)
        return;
    fprintf(file, "|%-15s\t|%-20s\t|%-20s\t|%-20s\t\n", "ID", "SURNAME", "NAME", "SALARY");
    for(int i = 0; i < cnt; i++)
    {
        fprintf(file,"|%-15u\t|%-20s\t|%-20s\t|%.5lf\t", 
            list[i].id,  list[i].surname, list[i].name, list[i].salary);
        fprintf(file, "\n");
    }
}


const char usage[] = "<in_file_path> -<flag> <out_file_path>\n";
const char help[] = "Commands:\n2) sort_asc sort_dsc\n";

int main(int argc, char** argv)
{
    printf(help);
    argc = 4;
    argv[1] = "in1.txt";
    argv[2] = "-d";
    argv[3] = "out1.txt";
    if(argc != 4)
    {
        show_error(INPUT_ERROR);
        return INPUT_ERROR;
    }
    if(!strcmp(argv[1],argv[3]))
    {
        show_error(INPUT_ERROR);
        return INPUT_ERROR;
    }
    status_code error = OK;
    error = check_flags(argc, argv);
    error = EPS > 0 ? error : INVALID_PARAMETER;
    if(error)
        return error;
    FILE* in = fopen(argv[1], "r");
    if(!in)
        return FILE_ERROR;
    FILE* out = fopen(argv[3], "w");
    if(!out)
    {
        fclose(in);
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    Employee* list;
    int list_count = 0;
    error = employees_parse(in, &list, &list_count);
    if(error)
    {
        free(list);
        fclose_all(2, in, out);
        return error;
    }
    char* buf = NULL;
    if((error = read_string(&buf)) != OK)
    {
        show_error(error);
        free_all(2, buf, list);
        fclose_all(2, in, out);
        return error;
    }
    //SORTING
    else if(!strcmp(buf, "sort_asc"))
    {   
        qsort(list, list_count, sizeof(Employee), cmp_asc);
        print_empllist(out, list, list_count);
        printf("Employees are sorted and printed\n");
        free_all(2, buf, list);
        fclose_all(2, in, out);
        return error;
    }
    else if(!strcmp(buf, "sort_dsc"))
    {   
        qsort(list, list_count, sizeof(Employee), cmp_dsc);
        print_empllist(out, list, list_count);
        printf("Employees are sorted and printed\n");
        free_all(2, buf, list);
        fclose_all(2, in, out);
        return error;
    }
    else
    {
        printf("Unknown command\n");
        fclose_all(2, in, out);
        free(buf);
        free(list);
        return OK;
    }
    if(error)
    {
        show_error(error);
    }
    fclose_all(2, in, out);
    free(buf);
    free(list);
    return error;  
}