#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <stdbool.h>

#define plus(x, y, z) __builtin_add_overflow(x, y, z)
#define mult(x, y, z) __builtin_mul_overflow(x, y, z)
#define sub(x,y,z) __builtin_sub_overflow(x, y, z)


typedef long long ll;

typedef enum status_code
{
    OK,
    INPUT_ERROR,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    ALLOC_ERROR,
    FILE_ERROR,
    INVALID_BRACKETS,
    UNDEFINED_RESULT,
    NEGATIVE_POW,
    NEGATIVE_MOD,
    EXTRA_OPERS,
    UNKNOWN_ERROR,
    NOTHING,
} status_code;

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Input error\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Invalid brackets\n",
    "ERROR: Undefined result\n",
    "ERROR: Negative pow\n",
    "ERROR: Negative mod\n",
    "ERROR: Extra operand or operator\n",
    "ERROR: Unknown...\n",
    "ERROR: Empty...?\n"
};

int show_error(status_code code)
{
    printf("%s", function_base_errors[code]);
    return code;
}

typedef struct Stack_c
{
    char data;
    struct Stack_c *next;
} Stack_c;

typedef struct Stack_n
{
    ll data;
    struct Stack_n *next;
} Stack_n;

//-------------------------------

status_code stack_push_c(Stack_c** st, char data)
{
    Stack_c* cur = (Stack_c*) calloc(1, sizeof(Stack_c));
    if(!st)
        return ALLOC_ERROR;
    cur->data = data;
    cur->next = *st;
    *st = cur;
    return OK;
}

status_code stack_pop_c(Stack_c** st, char *res)
{
    if (*st == NULL)
    {
        return INPUT_ERROR;
    }
    Stack_c* prev = *st;
    *st = (*st)->next;
    *res = prev->data;
    free(prev);
    return OK;
}

void destroy_stack_c(Stack_c* st)
{
    Stack_c* cur = NULL;
    while (st != NULL)
    {
        cur = st;
        st = st->next;
        free(cur);
    }
}

//------------------------------

status_code stack_push_n(Stack_n** st, ll data)
{
    Stack_n* cur = (Stack_n*) calloc(1, sizeof(Stack_n));
    if(!st)
        return ALLOC_ERROR;
    cur->data = data;
    cur->next = *st;
    *st = cur;
    return OK;
}

status_code stack_pop_n(Stack_n** st, ll *res)
{
    if (*st == NULL)
    {
        return INPUT_ERROR;
    }
    Stack_n* prev = *st;
    *st = (*st)->next;
    *res = prev->data;
    free(prev);
    return OK;
}

void destroy_stack_n(Stack_n* st)
{
    Stack_n* cur = NULL;
    while (st != NULL)
    {
        cur = st;
        st = st->next;
        free(cur);
    }
}

//------------------------------

status_code str_to_ll(char** in, ll* out, int base)
{
    errno = 0;
    char* endptr = NULL;
    *out = strtoll(*in, &endptr, base);
    if(errno == ERANGE)
        return MY_OVERFLOW;

    if (errno != 0 && *out == 0)
        return UNKNOWN_ERROR;
    
    if(*in == endptr)
        return INPUT_ERROR;
    
    if(*endptr != '\0')
        return INPUT_ERROR;

    return OK;
}

int is_op(char c)
{
    char symbs[] = "()+-*/%^~";
    for(int i = 0; i < strlen(symbs); i++)
    {
        if(symbs[i] == c)
            return true;
    }
    return false;
}

int priority(char c)
{
    switch (c) 
    {
        case '^':
            return 3;
        case '*':
        case '/':
        case '~':
        case '%':
            return 2;
        case '+':
        case '-':
            return 1;
        default:
            return 0;
    }
}

status_code bin_pow(ll a, ll n, ll* res) {
    if ((n == 0 && a == 0) || (n < 0) || (a == LLONG_MIN))
    {
        return INPUT_ERROR;
    }

    ll multiplier = 1;
    bool overflow = false;

    //Работа с модулем
    *res = 1;
    if (a < 0 && (n & 1 == 1))
    {
        multiplier = -1;
    }
    a = abs(a);

    while (n) 
    {
        if (n & 1)
        {
            overflow =  overflow || mult(*res, a, res);
        }
        overflow =  overflow || mult(a,a,&a);
        n >>= 1;
    }

    if (!overflow)
    {
        *res = *res * multiplier;
        return OK;
    }
    else
    {
        return MY_OVERFLOW;
    }
}

//
status_code infix_to_postfix(char* infix, char** result)
{
    if(infix == NULL)
        return INPUT_ERROR;
    int infix_cnt = strlen(infix);
    int postfix_size = 0;
    Stack_c* st = NULL;
    char* postfix = (char*) malloc(sizeof(char) * (strlen(infix) * 2 + 1));
    if(!postfix)
        return ALLOC_ERROR;

    for (int i = 0; i < infix_cnt; ++i)
    {

        if (isdigit(infix[i]))
        {
            postfix[postfix_size++] = infix[i];
            if (i < infix_cnt - 1 && !isdigit(infix[i + 1]))
                postfix[postfix_size++] = ' ';
            if(infix[i + 1] == '\0')
                postfix[postfix_size++] = ' ';
        }
        else if (infix[i] == '(')
        {
            if (i < infix_cnt - 1 && infix[i + 1] == ')')
                return INVALID_BRACKETS;
            stack_push_c(&st, infix[i]);
        }
        else if (infix[i] == ')')
        {
            char cur;
            while (st != NULL && st->data != '(')
            {
                postfix[postfix_size++] = st->data;
                postfix[postfix_size++] = ' ';
                stack_pop_c(&st, &cur);
            }

            if (st != NULL && st->data == '(')
                stack_pop_c(&st, &cur);
            else
                return INVALID_BRACKETS;
        }
        else if (is_op(infix[i]))
        {
            char cur;
            while (st != NULL && priority(infix[i]) <= priority(st->data))
            {
                postfix[postfix_size++] = st->data;
                postfix[postfix_size++] = ' ';
                stack_pop_c(&st, &cur);
            }
            stack_push_c(&st, infix[i]);
        }
        else
        {
            return INPUT_ERROR;
        }
    }

    while (st != NULL && st->data != '(')
    {
        char cur;
        postfix[postfix_size++] = st->data;
        postfix[postfix_size++] = ' ';
        stack_pop_c(&st, &cur);
    }

    if (st != NULL)
    {
        destroy_stack_c(st);
        return INVALID_BRACKETS;
    }
    if(postfix_size != 0 && postfix[postfix_size] == ' ')
        postfix[postfix_size - 1] = '\0';
    else
        postfix[postfix_size] = '\0';
    *result = postfix;
    return OK;
}

//
status_code calculation(char* postfix, ll* res)
{
    Stack_n* st = NULL;
    int postfix_size = strlen(postfix);
    char* buff = (char*) malloc(sizeof(char) * (postfix_size + 1));
    if (!buff)
        return ALLOC_ERROR;

    int buff_size = 0;
    int is_num = 0;
    (*res) = 0;
    int first = 1;

    for (int i = 0; i < postfix_size; i++)
    {
        if (isdigit(postfix[i]))
        {
            buff[buff_size++] = postfix[i];
            is_num = 1;
        }
        else if ((postfix[i] == ' ' || postfix[i] == '\0') && is_num)
        {
            status_code code = OK;
            buff[buff_size] = '\0';
            code = str_to_ll(&buff, &(*res), 10);
            if(code)
            {
                free(buff);
                return code;
            }
            if(postfix[i] == '\0' && first)
            {
                free(buff);
                return OK;
            }
            stack_push_n(&st, (*res));
            buff_size = 0;
            is_num = 0;
            first = 0;
        }
        else if (is_op(postfix[i]))
        {
            ll arg1, arg2;

            if (stack_pop_n(&st, &arg1) == INPUT_ERROR)
            {
                free(buff);
                destroy_stack_n(st);
                return EXTRA_OPERS;
            }

            if (postfix[i] == '~')
            {
                (*res) = -arg1;
            }
            else
            {
                if (stack_pop_n(&st, &arg2) == INPUT_ERROR)
                {
                    free(buff);
                    destroy_stack_n(st);
                    return EXTRA_OPERS;
                }
                bool overfl = false;
                switch (postfix[i])
                {
                    case '+':
                        overfl = plus(arg1, arg2, res);
                        if(overfl)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return MY_OVERFLOW;
                        }
                        break;
                    case '-':
                        overfl = sub(arg1, arg2, res);
                        if(overfl)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return MY_OVERFLOW;
                        }
                        break;
                    case '*':
                        overfl = mult(arg1, arg2, res);
                        if(overfl)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return MY_OVERFLOW;
                        }
                        break;
                    case '/':
                        if (arg1 == 0)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return DIVISION_BY_ZERO;
                        }
                        (*res) = arg2 / arg1;
                        break;
                    case '%':
                        if (arg1 == 0)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return DIVISION_BY_ZERO;
                        }
                        if (arg1 < 0 || arg2 < 0)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return NEGATIVE_MOD;
                        }
                        (*res) = arg2 % arg1;
                        break;
                    case '^':
                        if (arg1 < 0)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return NEGATIVE_POW;
                        }
                        if (arg1 == 0 && arg2 == 0)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return NEGATIVE_POW;
                        }
                        status_code code = OK;
                        code = bin_pow(arg2, arg1, res);
                        if(code)
                        {
                            free(buff);
                            destroy_stack_n(st);
                            return code;
                        }
                        break;
                    default:
                        free(buff);
                        destroy_stack_n(st);
                        return INPUT_ERROR;
                }
            }
            if (stack_push_n(&st, (*res)) == ALLOC_ERROR)
            {
                free(buff);
                destroy_stack_n(st);
                return ALLOC_ERROR;
            }
        }
    }

    if (stack_pop_n(&st, &(*res)) == INPUT_ERROR)
    {
        free(buff);
        destroy_stack_n(st);
        return EXTRA_OPERS;
    }

    destroy_stack_n(st);
    free(buff);

    return OK;
}

status_code err_file(const char * filename, char ** file_error)
{
    char * prefix = "err_";
    *file_error = (char *)malloc(sizeof(char) * (strlen(filename) + strlen(prefix) + 1));

    if (*file_error == NULL)
        return ALLOC_ERROR;

    (*file_error)[0] = '\0';
    strcat(*file_error, prefix);
    strcat(*file_error, filename);
    return OK;
}

status_code fread_line(FILE* file, char** res)
{
    if(!file || res == NULL)
        return INPUT_ERROR;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    ch = fgetc(file);


    while (ch != '\0' && ch != '\n' && ch != EOF)
    {
        if (size == max_size - 1) 
        {
            max_size *= 2;
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
        ch = fgetc(file);
    }
    *res = result;
    return OK;
}


int main(int argc, char * argv[])
{
    argc = 3;
    argv[1] = "f1";
    argv[2] = "f2";
    if(argc == 1)
    {
        printf("Write paths to files\n");
        return INPUT_ERROR;
    }

    FILE* input = NULL;
    FILE* error_file = NULL;

    char* infix = (char*) malloc(sizeof(char) * 1024);
    if(infix == NULL)
    {
        show_error(ALLOC_ERROR);
        return ALLOC_ERROR;
    }

    char * err_filename = NULL;

    int count_line = 0;
    status_code error = OK;
    for (int i = 1; i < argc; ++i)
    {
        input = fopen(argv[i], "r");
        if(input == NULL)
        {
            show_error(FILE_ERROR);
            printf("file: %d does not exist", i);
            continue;
        }
        printf("file %d results\n", i);

        if((error = err_file(argv[i], &err_filename)) != OK)
        {
            show_error(error);
            free(infix);
            fclose(input);
            return error;
        }

        while (!feof(input))
        {
            error = fread_line(input, &infix);
            if(error == ALLOC_ERROR)
            {
                show_error(error);
                fclose(input);
                free(infix);
                free(err_filename);
                return error;
            }
            else if(error)
            {
                if(error_file == NULL)
                {
                    error_file = fopen(err_filename, "w");
                    if(error_file == NULL)
                    {
                        show_error(FILE_ERROR);
                        fclose(input);
                        free(infix);
                        free(err_filename);
                        return FILE_ERROR;
                    }
                }
                fprintf(error_file, "%s in %d\n", function_base_errors[error], count_line);
                count_line++;
                free(err_filename);
                err_filename = NULL;
                continue;
            }

            char* postfix = (char*) malloc(sizeof(char) * (strlen(infix)*2));
            if(postfix == NULL)
            {
                fclose(input);
                free(infix);
                free(err_filename);
                return ALLOC_ERROR;
            }

            error = infix_to_postfix(infix, &postfix);
            if(error != OK)
            {
                if(error_file == NULL)
                {
                    error_file = fopen(err_filename, "w");
                    if(error_file == NULL)
                    {
                        show_error(FILE_ERROR);
                        fclose(input);
                        free(infix);
                        free(postfix);
                        free(err_filename);
                        return FILE_ERROR;
                    }
                }
                fprintf(error_file, "%s in %d\n", function_base_errors[error], count_line);
                count_line++;
                free(err_filename);
                err_filename = NULL;
                continue;
            }

            ll result = 0;
            error = calculation(postfix, &result);
            if(error != OK)
            {
                if(error_file == NULL)
                {
                    error_file = fopen(err_filename, "w");
                    if(error_file == NULL)
                    {
                        show_error(FILE_ERROR);
                        fclose(input);
                        free(infix);
                        free(postfix);
                        free(err_filename);
                        return FILE_ERROR;
                    }
                }
                fprintf(error_file, "%s in %d\n", function_base_errors[error], count_line);
                count_line++;
                free(err_filename);
                err_filename = NULL;
                continue;
            }

            printf("line: %15d|infix:%20s|postfix:%20s|result: %15lld\n", count_line, infix, postfix, result);
            count_line++;
            free(postfix);
        }

        if(error_file != NULL)
        {
            printf("Errors found! Look into err_%s\n", argv[i]);
            fclose(error_file);
            error_file = NULL;
        }

        count_line = 0;
        fclose(input);
        free(err_filename);
    }

    free(infix);
    return 0;
}