#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
//#include "String.h"

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

const char* error_details[] =
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

typedef struct 
{
    int size;
    char* data;
} String, *String_ptr;

int show_error(status_code code)
{
    printf("%s", error_details[code]);
    return code;
}

status_code string_create(const char* data, String_ptr result)
{
    if(data == NULL)
        return INVALID_PARAMETER;
    String res;
    res.size = strlen(data);
    res.data = (char*) malloc(sizeof(char) * (res.size + 1));
    if(!(res.data))
        return ALLOC_ERROR;
    strcpy(res.data, data);
    *result = res;
    return OK;
}

status_code string_create_from(const String_ptr str, String_ptr result)
{
    if(str == NULL)
        return INVALID_PARAMETER;
    String res;
    status_code error = string_create(str->data, &res);
    *result = res;
    return error;
}

void string_destroy(String_ptr ptr)
{
    if(ptr == NULL)
        return;
    if(ptr->data != NULL)
        free(ptr->data); 
    ptr->data = NULL;
    ptr = NULL;
}

status_code string_concat(const String_ptr str1, const String_ptr str2, String_ptr result)
{
    if(str1 == NULL || str2 == NULL)
        return INVALID_PARAMETER;
    String res;
    int n1 = str1->size;
    int n2 = str2->size;
    res.size = n1 + n2 + 1;
    res.data = (char*) malloc(sizeof(char) * (n1 + n2 + 1));
    if(res.data == NULL)
    {
        string_destroy(&res);
        return ALLOC_ERROR;
    }
    for(int i = 0; i < n1; i++)
    {
        res.data[i] = str1->data[i];
    }
    for(int i = 0; i < n2; i++)
    {
        res.data[i + n1] = str2->data[i];
    }
    res.data[n1 + n2] = '\0';
    *result = res;
    return OK;
}
//First greater if failed
int string_compare(const void* str1v, const void* str2v)
{
    String* str1 = (String*) str1v;
    String* str2 = (String*) str2v;
    if(str1 == NULL && str2 == NULL)
        return 0;
    if(str1 == NULL || str2 == NULL || str1->data == NULL || str2->data == NULL)
        return 1;
    int res = str1->size == str2->size ? 0 : 
        (str1->size - str2->size > 0 ? 1 : -1);
    if(res == 0)
    {
        int i = 0;
        while(i != str1->size && str1->data[i] == str2->data[i])
            i++;
        if(i == str1->size)
            return 0;
        return str1->data[i] - str2->data[i] > 0 ? 1 : -1;
    }
    return res;
}

bool string_equal(const String_ptr str1, const String_ptr str2)
{
    return string_compare((void*)str1, (void*)str2) == 0 ? true : false;
}

status_code string_copy(String_ptr from_res, String_ptr to_res)
{
    String to;
    if(from_res == NULL)
    {
        return INVALID_PARAMETER; 
    }
    to.size = from_res->size;
    to.data = (char*) malloc(sizeof(char) * (to.size +1));
    if(!to.data)
        return ALLOC_ERROR;
    for(int i = 0; i < to.size; i++)
    {
        to.data[i] = from_res->data[i];
    }
    to.data[to.size] = '\0';
    return OK;
}

//Reading standart input
status_code string_readline(String_ptr res)
{
    const int MAX_INPUT_SIZE = 2048;
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
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

status_code string_readline_file(String_ptr res, FILE* in)
{
    if(in == NULL)
        return INVALID_PARAMETER;
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    ch = fgetc(in);
    if(ch == EOF)
    {
        free(result);
        res->data = NULL;
        return OK;
    }
    while (ch != '\0' && ch != '\n' && ch != EOF)
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
            if (max_size > MAX_INPUT_SIZE)
            {
                free(result);
                return ALLOC_ERROR;
            }
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
        ch = fgetc(in);
    }
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

bool is_sep(char c)
{
    return (c == '\n' || c == '\t' || c == ' ');
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

status_code string_read_word(String_ptr res)
{
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    while(is_sep(ch))
        ch = getchar();

    while (ch != '\0' && !is_sep(ch))
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
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

status_code string_read_word_file(String_ptr res, FILE* in)
{
    if(in == NULL)
        return INVALID_PARAMETER;
    const int MAX_INPUT_SIZE = 2048;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    while(is_sep(ch))
        ch = fgetc(in);
    if(ch == EOF)
    {
        free(result);
        res->data = NULL;
        return OK;
    }
    while (ch != '\0' && !is_sep(ch) && ch != EOF)
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
        ch = fgetc(in);
    }
    status_code error = OK;
    String res_str;
    error = string_create(result, &res_str);
    if(error)
    {
        free(result);
        string_destroy(&res_str);
        return error;
    }
    free(result);
    *res = res_str;
    return OK;
}

status_code str_to_uint(String_ptr in, unsigned int* out, int base)
{
    if(in == NULL)
        return INVALID_PARAMETER;
    if(in->data == NULL)
        return INVALID_PARAMETER;
    //Если есть минус
    if(!is_digit((in->data)[0], base))
        return INVALID_PARAMETER;
    errno = 0;
    char* endptr = NULL;
    *out = strtoul(in->data, &endptr, base);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(in->data == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

status_code str_to_int(String_ptr in, int* out, int base)
{
    errno = 0;
    char* endptr = NULL;
    if(in == NULL || in->data == NULL)
        return INVALID_PARAMETER;
    *out = strtol(in->data, &endptr, base);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(in->data == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

status_code str_to_d(String_ptr in, double* out, int base)
{
    errno = 0;
    char* endptr = NULL;
    if(in == NULL || in->data == NULL)
        return INVALID_PARAMETER;
    *out = strtod(in->data, &endptr);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(in->data == endptr)
        return INVALID_PARAMETER;
    
    if(*endptr != '\0')
        return INVALID_PARAMETER;

    return OK;
}

//[begin;end)
status_code sub_string(String_ptr source, int begin, int end, String_ptr result)
{
    if(begin < 0 || end > source->size)
        return INVALID_PARAMETER;
    char* data = (char*) malloc(sizeof(char) * (end - begin + 1));
    for(int i = begin; i < end; i++)
    {
        data[i - begin] = source->data[i];
    }
    data[end-begin] = '\0';
    status_code error = string_create(data, result);
    free(data);
    return error;
}

bool check_adress_index(String_ptr str)
{
    if(str == NULL || str->data == NULL || strlen(str->data) != 6)
        return false;
    for(int i = 0; i < str->size; i++)
    {
        if(!is_digit(str->data[i], 10))
            return false;
    }
    return true;
}

bool check_weight(double x)
{
    return x > 0 && abs(x - 1e-10) > 0; 
}

bool check_mail_id(String_ptr str)
{
    if(str == NULL || str->data == NULL || strlen(str->data) != 14)
        return false;
    for(int i = 0; i < str->size; i++)
    {
        if(!is_digit(str->data[i], 10))
            return false;
    }
    return true;
}











































































//#include "my_lib.h"
typedef unsigned long long ull;

typedef struct Address
{
    String city;
    String street;
    unsigned int house_number;
    String korpus;
    unsigned int flat_number;
    String index;
} Address, *Address_ptr;

typedef struct Mail
{
    Address address;
    double weight;
    String mail_id;
    String time_created;
    String time_received;
} Mail, *Mail_ptr;

typedef struct Mail_Arr
{
    Mail* data;
    int* was_sent;
    int size;
    int max_size;
} Mail_Arr, *Mail_Arr_ptr;

typedef struct Post
{
    Address post_address;
    Mail_Arr mails;
} Post, *Post_ptr;

void print_mail(Mail_ptr ptr)
{
    printf("Address city: %s\n", ptr->address.city.data);
    printf("Address street: %s\n", ptr->address.street.data);
    printf("Address house number: %d\n", ptr->address.house_number);
    printf("Address korpus: %s\n", ptr->address.korpus.data);
    printf("Address flat number: %d\n", ptr->address.flat_number);
    printf("Address index: %s\n", ptr->address.index.data);
    printf("Mail id: %s\n", ptr->mail_id.data);
    printf("Weight: %lf\n", ptr->weight);
    printf("Time created: %s\n", ptr->time_created.data);
    printf("Time received: %s\n", ptr->time_received.data);
}

status_code time_to_string(struct tm *time, String_ptr res) {
    char* data = (char*) calloc(20, sizeof(char));
    if (!data)
        return ALLOC_ERROR;
    sprintf(data, "%2d:%2d:%4d %2d:%2d:%2d",
            time->tm_mday, time->tm_mon + 1, time->tm_year + 1900,
            time->tm_hour, time->tm_min, time->tm_sec);
    data[19] = '\0';
    status_code error = string_create(data, res);
    if(error)
    {
        string_destroy(res);
        free(data);
        return error;
    }
    free(data);
    return OK;
}

status_code post_create(Post_ptr result)
{
    Post post;
    post.mails.size = 0;
    post.mails.max_size = 2;
    post.mails.data = (Mail*) calloc(2, sizeof(Mail));
    if(!post.mails.data)
    {
        return ALLOC_ERROR;
    }
    *result = post;
    return OK;
}

void free_address(Address* ad)
{
    string_destroy(&(ad->city));
    string_destroy(&(ad->street));
    string_destroy(&(ad->korpus));
    string_destroy(&(ad->index));
}

void free_mail(Mail* mail)
{
    free_address(&(mail->address));
    string_destroy(&(mail->mail_id));
    string_destroy(&(mail->time_created));
    string_destroy(&(mail->time_received));
}

void free_mail_arr(Mail_Arr_ptr mail_arr)
{
    for(int i = 0; i < mail_arr->size; i++)
    {
        free_mail(&(mail_arr->data[i]));
    }
    free(mail_arr->was_sent);
    free(mail_arr->data);
}

void free_post(Post* post)
{
    free_address(&(post->post_address));
    free_mail_arr(&(post->mails));
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

//--------------------------------------------------------------------------------
int mail_cmp(const void* am, const void* bm)
{
    Mail* a = (Mail*)am;
    Mail* b = (Mail*)bm;
    int res = string_compare((void*) &(a->address.index), (void*) &(b->address.index));
    if(!res)
        return string_compare((void*) &(a->mail_id), (void*) &(b->mail_id));
    return res;
}

int mail_create_cmp(const void* am, const void* bm)
{
    String y1, y2, m1, m2;
    Mail* a = (Mail*)am;
    Mail* b = (Mail*)bm;
    //get year

    char* year1 = (char*) calloc(5, sizeof(char));
    char* year2 = (char*) calloc(5, sizeof(char));
    char* mm1 = (char*) calloc(3, sizeof(char));
    char* mm2 = (char*) calloc(3, sizeof(char));
    if(!year1 || !year2 || !mm1 || !mm2)
    {
        free_all(4, year1, year2, mm1, mm2);
        return 1;
    }
    status_code error = sub_string(&(a->time_created), 6, 10, &y1);
    error = error ? error : sub_string(&(b->time_created), 6, 10, &y2);
    if(error)
    {
        string_destroy(&y1);
        string_destroy(&y2);
        free_all(4, year1, year2, mm1, mm2);
        return 1;
    }
    int res = string_compare((void*) &(y1), (void*) &(y2));
    if(res != 0)
    {
        string_destroy(&y1);
        string_destroy(&y2);
        string_destroy(&m1);
        string_destroy(&m2);
        free_all(4, year1, year2, mm1, mm2);
        return res;
    }
    error = sub_string(&(a->time_created), 3, 5, &m1);
    error = error ? error : sub_string(&(b->time_created), 3, 5, &m2);
    if(error)
    {
        string_destroy(&y1);
        string_destroy(&y2);
        string_destroy(&m1);
        string_destroy(&m2);
        free_all(4, year1, year2, mm1, mm2);
        return 1;
    }
    res = string_compare((void*) &(m1), (void*) &(m2));
    if(res != 0)
    {
        string_destroy(&y1);
        string_destroy(&y2);
        string_destroy(&m1);
        string_destroy(&m2);
        free_all(4, year1, year2, mm1, mm2);
        return res;
    }
    error = sub_string(&(a->time_created), 0, 2, &m1);
    error = error ? error : sub_string(&(b->time_created), 0, 2, &m2);
    if(error)
    {
        string_destroy(&y1);
        string_destroy(&y2);
        string_destroy(&m1);
        string_destroy(&m2);
        free_all(4, year1, year2, mm1, mm2);
        return 1;
    }
    res = string_compare((void*) &(m1), (void*) &(m2));
    if(res != 0)
    {
        string_destroy(&y1);
        string_destroy(&y2);
        string_destroy(&m1);
        string_destroy(&m2);
        free_all(4, year1, year2, mm1, mm2);
        return res;
    }
    String t1, t2;
    error = sub_string(&(a->time_created), 11, 19, &t1);
    error = error ? error : sub_string(&(b->time_created), 11, 19, &t2);
    res = string_compare((void*) &(t1), (void*) &(t2));
    string_destroy(&t1);
    string_destroy(&t2);
    string_destroy(&y1);
    string_destroy(&y2);
    string_destroy(&m1);
    string_destroy(&m2);
    free_all(4, year1, year2, mm1, mm2);
    return res;
}

status_code create_mail_arr(Mail_Arr_ptr result)
{
    Mail_Arr res;
    res.data = (Mail*) malloc(sizeof(Mail) * 2);
    if(!res.data)
        return ALLOC_ERROR;
    res.was_sent = (int*) malloc(sizeof(int) * 2);
    if(!res.was_sent)
    {
        free(res.data);
        return ALLOC_ERROR;
    }
    for(int i = 0; i < 2; i++)
        res.was_sent[i] = 0;
    res.max_size = 2;
    res.size = 0;
    *result = res;
    return OK;
}

status_code mail_arr_push(Mail_Arr_ptr arr, Mail mail)
{
    arr->size += 1;
    if (arr->size == arr->max_size - 1) 
    {
        arr->max_size *= 2;
        Mail_ptr copyto_str = (Mail_ptr) realloc(arr->data, arr->max_size * sizeof(Mail));
        if (copyto_str == NULL)
        {
            free_mail_arr(arr);
            return ALLOC_ERROR;
        }
        arr->data = copyto_str;
        
        int* cop_to = arr->was_sent;
        arr->was_sent = (int*) realloc(arr->was_sent, arr->max_size * sizeof(int));
        if(arr->was_sent == NULL)
        {
            free(cop_to);
            free_mail_arr(arr);
            return ALLOC_ERROR;
        }
        for(int i = arr->size; i < arr->max_size; i++)
            cop_to[i] = 0;
        arr->was_sent = cop_to;
    }
    arr->data[arr->size - 1] = mail;
    arr->was_sent[arr->size - 1] = 0;
    qsort(arr->data, arr->size, sizeof(Mail), mail_cmp);
    return OK;
}

status_code remove_mail(Mail_Arr_ptr arr, int index)
{
    if(index < 0 || index >= arr->size || arr == NULL)
        return INVALID_PARAMETER;
    for(int i = index; i < arr->size - 1; i++)
    {
        arr->data[i] = arr->data[i + 1];
    }
    free_mail(&(arr->data[arr->size]));
    arr->size -= 1;
    return OK;
}

void print_mails(Mail_Arr_ptr arr)
{
    if(arr == NULL || arr->data == 0 || arr->size == 0)
        return;
    for(int i = 0; i < arr->size; i++)
    {
        printf("Mail N%d: mail_id = %s, post_index = %s\n", 
            i+1, arr->data[i].mail_id.data, arr->data[i].address.index.data);
    }
}

void print_expired(Mail_Arr_ptr arr)
{
    if(arr == NULL || arr->data == 0 || arr->size == 0)
        return;
    qsort(arr->data, arr->size, sizeof(Mail), mail_create_cmp);
    time_t t = time(NULL);
    String time_now;
    status_code error = time_to_string(localtime(&t), &(time_now));
    if(error)
    {
        string_destroy(&time_now);
        return;
    }
    bool found = false;
    for(int i = 0; i < arr->size; i++)
    {
        if(!(arr->was_sent[i]) && string_compare((void*) &time_now, (void*) (&(arr->data[i].time_received))) > 0)
        {
            printf("Mail N%d: mail_id = %s, post_index = %s, create_time = %s\n", 
                i+1, arr->data[i].mail_id.data, arr->data[i].address.index.data, arr->data[i].time_created.data);
            found = true;
        }
    }
    if(!found)
        printf("Mails were not found\n");
    qsort(arr->data, arr->size,sizeof(Mail), mail_cmp);
}

void print_received(Mail_Arr_ptr arr)
{
    if(arr == NULL || arr->data == 0 || arr->size == 0)
        return;
    qsort(arr->data, arr->size, sizeof(Mail), mail_create_cmp);
    time_t t = time(NULL);
    String time_now;
    status_code error = time_to_string(localtime(&t), &(time_now));
    if(error)
    {
        string_destroy(&time_now);
        return;
    }
    bool found = false;
    for(int i = 0; i < arr->size; i++)
    {
        if(arr->was_sent[i])
        {
            printf("Mail N%d: mail_id = %s, post_index = %s, create_time = %s\n", 
            i+1, arr->data[i].mail_id.data, arr->data[i].address.index.data, arr->data[i].time_created.data);
            found = true;
        }
    }
    if(!found)
        printf("Mails were not found\n");
    qsort(arr->data, arr->size, sizeof(Mail), mail_cmp);
}

status_code add_mail(Post_ptr post, Mail_ptr mail)
{
    status_code error = OK;
    error = mail_arr_push(&(post->mails),*mail);
    if(error)
        return error;
    return OK;
}

status_code create_address(Address_ptr adr)
{
    String buffer;
    status_code error = OK;
    Address address;
    printf("Enter address info\n");
    printf("City: ");
    error = string_readline(&buffer);
    if(error)
    {
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    address.city = buffer;

    printf("Street: ");
    error = string_readline(&buffer);
    if(error)
    {
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    address.street = buffer;

    printf("House_number: ");
    error = string_readline(&buffer);
    unsigned int out;

    error = error ? error : str_to_uint(&buffer, &out, 10);
    if(error || out == 0)
    {
        if (out == 0) 
            error = INVALID_PARAMETER;
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    address.house_number = out;

    printf("Korpus: ");
    error = string_readline(&buffer);
    if(error)
    {
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    address.korpus = buffer;

    printf("Flat number: ");
    error = string_readline(&buffer);
    error = error ? error : str_to_uint(&buffer, &out, 10);
    if(error || out == 0)
    {
        if (out == 0) 
            error = INVALID_PARAMETER;
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    address.flat_number = out;

    printf("Index: ");
    error = string_readline(&buffer);
    if(error)
    {
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, "") || !check_adress_index(&buffer))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    address.index = buffer;
    *adr = address;
    string_destroy(&buffer);
    return OK;
}

status_code create_mail(Mail_ptr result)
{
    unsigned int out;
    String buffer;
    Mail mail;
    Address address;
    status_code error = create_address(&address);
    if(error)
    {
        free_address(&address);
        return error;
    }
    mail.address = address;
    printf("Weight: ");
    error = string_readline(&buffer);
    double outd;
    error = error ? error : str_to_d(&buffer, &outd, 10);

    if(error || !check_weight(outd))
    {
        if (!check_weight(outd)) 
            error = INVALID_PARAMETER;
        free_mail(&mail);
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    mail.weight = outd;

    printf("Mail Index: ");
    error = string_readline(&buffer);
    if(error)
    {
        free_mail(&mail);
        string_destroy(&buffer);
        return error;
    }
    if(!strcmp(buffer.data, ""))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    if(!check_mail_id(&buffer))
    {
        string_destroy(&buffer);
        return INVALID_PARAMETER;
    }
    mail.mail_id = buffer;

    printf("Print, how much (in seconds) should mail be received from now: ");
    error = string_readline(&buffer);
    error = error ? error : str_to_uint(&buffer, &out, 10);
    if(error && out == 0)
    {
        if (out == 0) 
                error = INVALID_PARAMETER;
        free_mail(&mail);
        string_destroy(&buffer);
        return error;
    }
    printf("Mail created!\n");
    time_t t = time(NULL);
    error = time_to_string(localtime(&t), &(mail.time_created));
    if(error)
    {
        free_mail(&mail);
        string_destroy(&buffer);
        return error;
    }
    t += out;
    error = time_to_string(localtime(&t), &(mail.time_received));
    if(error)
    {
        free_mail(&mail);
        string_destroy(&buffer);
        return error;
    }
    *result = mail;
    string_destroy(&buffer);
    return OK;
}

void printhelp()
{
    printf("Choose number of command:\n");
    printf("1.Create Mail and fill mail data\n");
    printf("2.Send Mail to a receiver (it'll be marked as received)\n");
    printf("3.Remove Mail from Post\n");
    printf("4.Show all Mails in the post\n");
    printf("5.Show details of the Mail\n");
    printf("6.Show all expired mails\n");
    printf("7.Show all received mails\n");
    printf("0.To exit the program\n");
}

int main(int argc, char** argv)
{
    status_code error = OK;
    Post post;
    error = post_create(&post);
    if(error)
    {
        show_error(error);
        free_post(&post);
        return error;
    }
    printf("Please, write your post address, it won't take a lot of time\n");
    error = create_address(&(post.post_address));
    if(error)
    {
        show_error(error);
        free_post(&post);
        return error;
    }
    printf("use \"help\" to print a list of commands\n");
    String buff;
    while(!error)
    {
        error = string_readline(&buff);
        if(error)
            break;
        int command = 0;
        if(!strcmp(buff.data, "help"))
        {
            printhelp();
        }
        else
        {
            error = str_to_int(&buff, &command, 10);
            if(error)
                break;
            
            if(command == 0)
                break;
            else if (command == 1)
            {
                Mail mail;
                error = create_mail(&mail);
                error = error ? error : add_mail(&post, &mail);
                if(error)
                {
                    break;
                }
                printf("Mail was successfully added\n");
            }
            else if (command == 2)
            {
                printf("Enter uid of mail\n");
                error = string_readline(&buff);
                if(error)
                    break;
                bool found = false;
                for(int i = 0; i < post.mails.size; i++)
                {
                    if(string_equal(&buff, &(post.mails.data[i].mail_id)))
                    {
                        post.mails.was_sent[i] = true;
                        found = true;
                    }
                }
                if(found)
                    printf("Mail was successfully sent\n");
                else    
                    printf("Mail was not found\n");
            }
            else if (command == 3)
            {
                int index = -1;
                printf("Enter uid of mail\n");
                error = string_readline(&buff);
                if(error)
                    break;
                for(int i = 0; i < post.mails.size; i++)
                {
                    if(string_equal(&buff, &(post.mails.data[i].mail_id)))
                    {
                        index = i;
                        break;
                    }
                }
                if(index == -1)
                    printf("Mail was not found\n");
                else
                {
                    error = remove_mail(&post.mails, index);
                    if(error)
                        break;
                    printf("Mail was successfully removed\n");
                }
            }
            else if (command == 4)
            {
                print_mails(&(post.mails));
            }
            else if (command == 5)
            {
                int index = -1;
                printf("Enter uid of mail\n");
                error = string_readline(&buff);
                if(error)
                    break;
                for(int i = 0; i < post.mails.size; i++)
                {
                    if(string_equal(&buff, &(post.mails.data[i].mail_id)))
                    {
                        index = i;
                        break;
                    }
                }
                if(index == -1)
                    printf("Mail was not found\n");
                else
                {
                    print_mail(&(post.mails.data[index]));
                }
            }
            else if (command == 6)
            {
                print_expired(&(post.mails));
            }
            else if (command == 7)
            {
                print_received(&(post.mails));
            }
        }
    }
    show_error(error);
    free_post(&post);
    if (!error)
        string_destroy(&buff);
    else
        show_error(error);
    return error;
}