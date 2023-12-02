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

typedef struct 
{
    unsigned int id;
    char surname[50];
    char name[50];
    char group[50];
    unsigned char* marks;
} Student;

typedef enum
{
    ID,
    SURNAME, 
    NAME,
    GROUP,
    ABOVE_AVERAGE_MARK
} Search_Param;

bool is_mark(char mark)
{
    if(mark >= 1 && mark <= 5)
        return 1;
    else
        return 0;
}
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

status_code str_copy(char** in, char** out)
{
    if(*in == NULL)
        return INVALID_PARAMETER;
    int len = strlen(*in);
    *out = (char*) malloc(len * sizeof(char));
    if(*out == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len; i++)
    {
        (*out)[i] = (*in)[i];
    }
    return OK;
}

status_code ustr_copy(unsigned char** in, unsigned char** out, int in_cnt)
{
    if(*in == NULL)
        return INVALID_PARAMETER;
    int len = in_cnt;
    *out = (unsigned char*) malloc(len * sizeof(unsigned char));
    if(*out == NULL)
        return ALLOC_ERROR;
    for(int i = 0; i < len; i++)
    {
        (*out)[i] = (*in)[i];
    }
    return OK;
}

void arrstr_copy(char* in, char* out, int in_cnt)
{
    for(int i = 0; i < in_cnt; i++)
    {
        out[i] = in[i];
    }
    out[in_cnt] = '\0'; 
}

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

bool is_digit(char input, int base)
{
    int d = get_digit(input);
    if(d == -1 || d >= base)
        return false;
    return true;
}

status_code str_to_uint(char** in, unsigned int* out, int base)
{
    if(in == NULL || *in == NULL)
        return INVALID_PARAMETER;

    //Если есть минус
    if(!is_digit((*in)[0], base))
        return INVALID_PARAMETER;
    errno = 0;
    char* endptr = NULL;
    *out = strtoul(*in, &endptr, base);
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
//--------------------------------------------------------------------------------
//Just some more

void student_free(Student stud)
{
    free(stud.marks);
}

void student_list_free(Student* list, int count)
{
    if(list == NULL)
        return;
    for(int i = 0; i < count; i++)
        student_free(list[i]);
    free(list);
}

status_code average_mark(Student* stud, double* result)
{
    *result = 0;
    if(stud == NULL || stud->marks == NULL)
        return INVALID_PARAMETER;
    for(int i = 0; i < EXAMS_COUNT; i++)
    {
        (*result) += (stud->marks)[i];
    }
    *result = *result / EXAMS_COUNT;
    return OK;
}

bool student_valid(Student* st)
{
    if(st == NULL || st->name == NULL || st->group == NULL || st->surname == NULL || st->marks == NULL)
        return false;
    for (int i = 0; i < strlen(st->surname); i++)
    {
        if(!isalpha(st->surname[i]))
            return false;
    }
    for (int i = 0; i < strlen(st->name); i++)
    {
        if(!isalpha(st->name[i]))
            return false;
    }
    //Допустим, оценки в экзамене в пределах [1, 5]
    for(int i = 0; i < EXAMS_COUNT; i++)
    {
        if(!is_mark(st->marks[i]))
            return false;
    }
    return true;
}

int cmp_id(const void* av, const void* bv)
{
    Student* a = (Student*) av;
    Student* b = (Student*) bv;
    if(a->id == b->id)
        return 0;
    else if(a->id > b->id)
        return 1;
    else 
        return -1;
}

int cmp_name(const void* av, const void* bv)
{
    Student* a = (Student*) av;
    Student* b = (Student*) bv;
    if(a == NULL || b == NULL || a->name == NULL || b->name == NULL)
        return 1;
    return strcmp(a->name, b->name);
}

int cmp_surname(const void* av, const void* bv)
{
    Student* a = (Student*) av;
    Student* b = (Student*) bv;
    if(a == NULL || b == NULL || a->name == NULL || b->name == NULL)
        return 1;
    return strcmp(a->surname, b->surname);
}

int cmp_group(const void* av, const void* bv)
{
    Student* a = (Student*) av;
    Student* b = (Student*) bv;
    if(a == NULL || b == NULL || a->name == NULL || b->name == NULL)
        return 1;
    return strcmp(a->group, b->group);
}

int cmp_int (const void* a, const void* b) {
   return *(int*)a - *(int*)b;
}

bool student_list_valid(Student* input_list, int count)
{
    unsigned int* ids = (unsigned int*)malloc(count * sizeof(unsigned int));
    if(ids == NULL)
        return ALLOC_ERROR;
    if(input_list == NULL)
        return false;
    for(int i = 0; i < count; i++)
    {
        if(!student_valid(&(input_list[i])))
            return false;
        ids[i] = input_list[i].id;
    }
    qsort(ids, count, sizeof(int), cmp_int);
    bool res = true;
    for(int i = 1; i < count; i++)
    {
        //not unique
        if(ids[i-1] == ids[i])
        {
            res = false;
            break;
        }
    }
    free(ids);
    return res;
}

//using student_list_valid
status_code students_parse(FILE* in, Student** result, int* result_count)
{
    status_code code = OK;
    if(in == NULL || result == NULL)
        return INVALID_PARAMETER;
    *result_count = 0;
    int result_maxsz = 1;
    Student* studs = (Student*) malloc(result_maxsz * sizeof(Student));
    Student* last_st = &(studs[0]);
    if(studs == NULL)
        return ALLOC_ERROR;
    while(!feof(in))
    {
        int g = last_st->id;
        last_st->marks = (unsigned char*) malloc((EXAMS_COUNT+1) * sizeof(unsigned char));
        if(last_st->marks == NULL)
        {
            code = ALLOC_ERROR;
            student_list_free(studs, *result_count);
            return code;
        }
        if(fscanf(in, "%u %50[^ ] %50[^ ] %50[^ ] %hhu %hhu %hhu %hhu %hhu\n",
            &(last_st->id), (last_st->surname), &(last_st->name), &(last_st->group), 
            &(last_st->marks[0]), &(last_st->marks[1]), &(last_st->marks[2]), &(last_st->marks[3]), &(last_st->marks[4])) != 9)
        {
            code = INPUT_ERROR;
            student_list_free(studs, *result_count);
            return code;
        }
        last_st->marks[5] = '\0';
        (*result_count)++;
        if(*result_count == result_maxsz)
        {
            result_maxsz *= 2;
            Student* copyto = (Student*) realloc(studs, sizeof(Student) * result_maxsz);
            if(copyto == NULL)
            {
                student_list_free(studs, *result_count);
                code = ALLOC_ERROR;
                return code;
            }
            else
            {
                studs = copyto;
            }
        }
        last_st = &(studs[*result_count]);
    }
    if(!student_list_valid(studs, *result_count))
    {
        student_list_free(studs, *result_count);
        return INPUT_ERROR;
    }
    *result = studs;
    return code;
}

status_code student_list_filter(Search_Param param, void* parameter, Student* input_list, int input_count, Student** result, int* result_count)
{
    *result_count = 0;
    
    status_code code = OK;
    Student* studs;
    if(input_list == NULL || *result == NULL || (param != ABOVE_AVERAGE_MARK && parameter == NULL))
        return INVALID_PARAMETER;
    if(!student_list_valid(input_list, input_count))
        return INVALID_PARAMETER;

    int result_maxsz = 1;
    studs = (Student*) malloc(result_maxsz * sizeof(Student));
    if(studs == NULL)
        return ALLOC_ERROR;

    double av_mark = 0;
    for(int i = 0; i< input_count; i++)
    {
        double stud_av = 0;
        for(int j = 0; j < EXAMS_COUNT; j++)
        {
            stud_av += input_list[i].marks[j];
        }
        stud_av /= EXAMS_COUNT;
        av_mark += stud_av;
    }
    av_mark /= input_count;
    for(int i = 0; i < input_count; i++)
    {
        bool add = false;
        if(param == ID)
        {
            int key = *(unsigned int*)parameter;
            if(key == input_list[i].id)
                add = true;
        }
        else if (param == SURNAME)
        {
            char* key = (char*)parameter;
            if(!strcmp(key,input_list[i].surname))
                add = true;
        }
        else if (param == NAME)
        {
            char* key = (char*)parameter;
            if(!strcmp(key, input_list[i].name))
                add = true;
        }
        else if (param == GROUP)
        {
            char* key = (char*)parameter;
            if(!strcmp(key,input_list[i].group))
                add = true;
        }
        else if(param == ABOVE_AVERAGE_MARK)
        {
            double stud_av = 0;
            average_mark(&(input_list[i]), &stud_av);
            if(stud_av > av_mark)
                add = true;
        }
        else
        {
            code = INVALID_PARAMETER;
            break;
        }

        if(add)
        {
            add = false;
            (*result_count)++;
            studs[*result_count - 1] = input_list[i];
        }

        if(*result_count == result_maxsz)
        {
            result_maxsz *= 2;
            Student* copyto = (Student*) realloc(studs, result_maxsz * sizeof(Student));
            if(copyto == NULL)
            {
                free(*result);
                code = ALLOC_ERROR;
                return code;
            }
            studs = copyto;
        }
    }
    *result = studs;
    return code;
}

void print_studlist(Student* list, int cnt)
{
    if(list == NULL)
        return;
    if(cnt == 0)
    {
        printf("No one found\n");
        return;
    }
    for(int i = 0; i < cnt; i++)
    {
        if(!student_valid(&list[i]))
            return;
    }
    printf("|%-15s\t|%-20s\t|%-20s\t|%-20s\t|MARKS\t\n", "ID", "SURNAME", "NAME", "GROUP", "MARKS");
    for(int i = 0; i < cnt; i++)
    {
        printf("|%-15u\t|%-20s\t|%-20s\t|%-20s\t|", 
            list[i].id,  list[i].surname, list[i].name, list[i].group);
        for(int j = 0; j < EXAMS_COUNT; j++)
        {
            printf("%hhu ", list[i].marks[j]);
        }
        printf("\n");
    }
}

const char usage[] = "<in_file_path> <out_file_path>\n";
const char help[] = "Commands:\n1) srch_id srch_surname srch_name srch_group\n2) sort_id sort_surname sort_name sort_group\n3) print_curlist\n4) fprint_above_av fprint_stud_info\n";

int main(int argc, char** argv)
{
    status_code code = OK;
    if(argc != 3)
    {
        printf(usage);
        return INPUT_ERROR;
    }
    FILE* in = fopen(argv[1], "r");
    if(in == NULL)
    {
        code = FILE_ERROR;
        show_error(code);
        return code;
    }
    FILE* out = fopen(argv[2], "w");
    if(out == NULL)
    {
        fclose(in);
        code = FILE_ERROR;
        show_error(code);
        return code;
    }
    bool working = true;
    char* buf = NULL;
    Student* list = NULL;
    Student* filter_result = NULL;
    int list_cnt = 0;
    int filter_result_cnt = 0;
    code = code ? code : students_parse(in, &list, &list_cnt);
    while(code == OK)
    {
        if((code = read_string(&buf)) != OK)
            break;
        //FILTERING
        else if(!strcmp(buf, "srch_id"))
        {
            printf("Enter search id:\n");
            code = code ? code : read_string(&buf);
            unsigned int id = 0;
            code = code ? code : str_to_uint(&buf, &id, 10);
            code = code ? code : student_list_filter(ID, (void*)(&id), list, list_cnt, &filter_result, &filter_result_cnt);
            if(code == OK)
            {
                if(filter_result_cnt == 0)
                    printf("No one found. Try again\n");
                else
                    print_studlist(filter_result, filter_result_cnt);
            }
        }
        else if(!strcmp(buf, "srch_surname"))
        {   
            printf("Enter search surname:\n");
            code = code ? code : read_string(&buf);
            code = code ? code : student_list_filter(SURNAME, buf, list, list_cnt, &filter_result, &filter_result_cnt);
            if(code == OK)
            {
                if(filter_result_cnt == 0)
                    printf("No one found. Try again\n");
                else
                    print_studlist(filter_result, filter_result_cnt);
            }
        }
        else if(!strcmp(buf, "srch_name"))
        {   
            printf("Enter search name:\n");
            code = code ? code : read_string(&buf);
            code = code ? code : student_list_filter(NAME, buf, list, list_cnt, &filter_result, &filter_result_cnt);
            if(code == OK)
            {
                if(filter_result_cnt == 0)
                    printf("No one found. Try again\n");
                else
                    print_studlist(filter_result, filter_result_cnt);
            }
        }
        else if(!strcmp(buf, "srch_group"))
        {   
            printf("Enter search group:\n");
            code = code ? code : read_string(&buf);
            code = code ? code : student_list_filter(GROUP, buf, list, list_cnt, &filter_result, &filter_result_cnt);
            if(code == OK)
            {
                if(filter_result_cnt == 0)
                    printf("No one found. Try again\n");
                else
                    print_studlist(filter_result, filter_result_cnt);
            }
        }
        //SORTING
        else if(!strcmp(buf, "sort_id"))
        {   
            qsort(list, list_cnt, sizeof(Student), cmp_id);
            printf("Students in buffer are sorted\n");
        }
        else if(!strcmp(buf, "sort_surname"))
        {   
            qsort(list, list_cnt, sizeof(Student), cmp_surname);
            printf("Students in buffer are sorted\n");
        }
        else if(!strcmp(buf, "sort_name"))
        {   
            qsort(list, list_cnt, sizeof(Student), cmp_name);
            printf("Students in buffer are sorted\n");
        }
        else if(!strcmp(buf, "sort_group"))
        {   
            qsort(list, list_cnt, sizeof(Student), cmp_group);
            printf("Students in buffer are sorted\n");
        }
        //PRINTING
        else if(!strcmp(buf, "print_curlist"))
        {   
            print_studlist(list, list_cnt);
        }
        else if(!strcmp(buf, "fprint_above_av"))
        {
            double average = 0;
            for(int i = 0; i < list_cnt; i++)
            {
                double res = 0;
                average_mark(&list[i], &res);
                average += res;
            }
            average /= list_cnt;
            code = code ? code : student_list_filter(ABOVE_AVERAGE_MARK, NULL, list, list_cnt, &filter_result, &filter_result_cnt);
            printf("Average mark: %.5lf. %d students passed over\n", average, filter_result_cnt);
            if(!code)
            {
                if(filter_result_cnt != 0)
                {
                    for(int i = 0; i < filter_result_cnt; i++)
                    {
                        double result = 0;
                        code = code ? code : average_mark(&filter_result[i], &result);
                        fprintf(out, "Found: %s %s, average mark: %.5lf\n", filter_result[i].surname, filter_result[i].name, result);
                    }
                }
            }
        }
        else if(!strcmp(buf, "fprint_stud_info"))
        {   
            printf("Enter student id:\n");
            code = code ? code : read_string(&buf);
            unsigned int id = 0;
            code = code ? code : str_to_uint(&buf, &id, 10);
            code = code ? code : student_list_filter(ID, (void*)&id, list, list_cnt, &filter_result, &filter_result_cnt);
            if(!code)
            {
                if(filter_result_cnt == 0)
                {
                    printf("No one found. Output file is empty\n");
                }
                else
                {
                    Student* cur = &filter_result[0];
                    double result = 0;
                    code = code ? code : average_mark(cur, &result);
                    fprintf(out, "Found: %s %s, average mark: %.5lf\n", cur->surname, cur->name, result);
                }
            }
        }
        else if(!strcmp(buf, "exit"))
        {
            break;
        }
        else if(!strcmp(buf, "help"))
        {
            printf(help);
        }
        else
        {
            printf("Unknown command. Use \"help\"\n");
            printf("Or print \"exit\" to close program\n");
        }
    }
    if(code != OK)
    {
        show_error(code);
    }
    fclose_all(2, in, out);
    free(buf);
    student_list_free(list, list_cnt);
    printf("Press any char to close program\n");
    getchar();
    return code;
}