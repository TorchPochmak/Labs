#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef unsigned long long int ull;
typedef long long int ll;
typedef unsigned char uchar;

#define MAX_INPUT_SIZE 1024

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
    WRONG_FORMAT,
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
    "ERROR: Wrong format...\n"
};

int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}

status_code fread_string(FILE* f, char** res)
{
    if(f == NULL)
        return FILE_ERROR;
    int max_size = 2;
    int size = 1;
    char ch = ' ';

    char* result = (char*) calloc(size + 1, sizeof(char));
    if (result == NULL)
        return ALLOC_ERROR;

    ch = fgetc(f);


    while (ch != '\0' && ch != '\n' && ch != EOF)
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
        ch = fgetc(f);
    }
    *res = result;
    return OK;
}
//Just some more

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

//---------------------------------------------------------------------
typedef struct Node
{
    struct Node* child;
    struct Node* brother;
    struct Node* parent;
    char letter;
} Node;

status_code create_node(char content, Node** out_node)
{
    if(out_node == NULL)
        return INVALID_PARAMETER;
    *out_node = (Node*) malloc(sizeof(Node));
    if(*out_node == NULL)
        return ALLOC_ERROR;

    (*out_node)->parent = NULL;
    (*out_node)->child = NULL;
    (*out_node)->brother = NULL;
    (*out_node)->letter = content;
    return OK;
}

status_code create_child(Node* parent, char content, Node** out_child)
{
    status_code code = OK;
    if(parent == NULL)
        return INVALID_PARAMETER;
    Node* new_child;
    if((code = create_node(content, &new_child)) != OK)
        return code;
    new_child->parent = parent;
    if(parent->child == NULL)
        parent->child = new_child;
    else
    {
        Node* last_brother = parent->child;
        while(last_brother->brother != NULL)
            last_brother = last_brother->brother;
        last_brother->brother = new_child;
    }
    *out_child = new_child;
    return OK;
}

void print_tree(Node* root, FILE* out, int spaces)
{
    if(root == NULL || out == NULL)
        return;
    for(int i = 0; i < spaces; i++)
        fprintf(out, " ");

    fprintf(out, "%c ", root->letter);
    fprintf(out, "\n");
    if(root->child != NULL)
    {
        print_tree(root->child, out, spaces + 1);
    }
    if(root->brother != NULL)
    {
        print_tree(root->brother, out, spaces);
    }
}
//str is good guaranteed
status_code create_tree(char* str, Node** root)
{
    status_code code = OK;
    Node* tree;
    Node* cur_node;
    int len = strlen(str);
    int mode = 0;//0 - add brother, 1 - add child;
    for(int i = 0; i < len; i++)
    {
        if(str[i] == ' ')
            continue;
        if(i == 0)
        {
            if(isalpha(str[i]))
            {
                if((code = create_node(str[i], &tree)) != OK)
                {
                    free(str);
                    return code;
                }
                *root = tree;
                cur_node = tree;
            }
        }
        else
        {
            if(str[i] == '(')
                mode = 1;
            else if(str[i] == ',')
                mode = 0;
            else if(str[i] == ')')
                cur_node = cur_node->parent;
            else
            {
                if(mode == 0)
                {
                    Node* brother;
                    if((code = create_node(str[i], &brother)) != OK)
                    {
                        show_error(code);
                        free(str);
                        return code;
                    }
                    cur_node->brother = brother;
                    cur_node->brother->parent = cur_node->parent;
                    cur_node = brother;
                }
                if(mode == 1)
                {
                    if((code = create_child(cur_node, str[i], &cur_node)) != OK)
                    {
                        free(str);
                        return code;
                    }
                }
            }
        }
    }
    return OK;
}

void free_tree(Node* root)
{
    if(root->child != NULL)
        free_tree(root->child);
    if(root->brother != NULL)
        free_tree(root->brother);
    free(root);
}

int main(int argc, char** argv)
{
    status_code code = OK;
    if(argc != 3)
    {
        printf("Wrong count of arguments");
        return INVALID_PARAMETER;
    }
    FILE* in;
    FILE* out;
    if((in = fopen(argv[1], "r")) == NULL)
    {
        code = FILE_ERROR;
        show_error(code);
        return code;
    }
    if((out = fopen(argv[2], "w")) == NULL)
    {
        fclose(in);
        code = FILE_ERROR;
        show_error(code);
        return code;
    }
    char* str = " ";
    Node* root;
    while(!feof(in))
    {
        if((code = fread_string(in, &str)) != OK)
        {
            show_error(code);
            fclose_all(2, in, out);
            free(str);
            free_tree(root);
            return code;
        }
        if((code = create_tree(str, &root)) != OK)
        {
            show_error(code);
            fclose_all(2, in, out);
            free(str);
            free_tree(root);
            return code;
        }
        print_tree(root, out, 0);
        fprintf(out, "\n");
    }
    fclose_all(2, in, out);
    free(str);
    free_tree(root);
    return OK;
}