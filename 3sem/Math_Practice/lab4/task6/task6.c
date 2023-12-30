#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


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
    "ERROR: Input error\n",
    "ERROR: Unknown...\n",
    "ERROR: Reserved...\n"
};

int show_error(status_code code)
{
    printf("%s", function_base_errors[code]);
    return code;
}

typedef enum
{
    TMP = 0,
    CNST,
    CONJ,
    DISJ,
    INV,
    IMPL,
    COMPL,
    SUMMOD2,
    EQUIV,
    SHEFFER,
    VEBB,
    LEFTSK,
    RIGHTSK,
    UNKNOWN
} token;

token get_token(char* data)
{
    if(data == NULL || strlen(data) > 2)
        return UNKNOWN;
    if(strlen(data) == 1)
    {
        if(data[0] == '0' || data[0] == '1')
            return CNST;
        if(toupper(data[0]) >= 'A' && toupper(data[0]) <= 'Z')
            return TMP;
        if(data[0] == '&')
            return CONJ;
        if(data[0] == '|')
            return DISJ;
        if(data[0] == '~')
            return INV;
        if(data[0] == '!')
           return SHEFFER;
        if(data[0] == '?')
            return VEBB;
        if(data[0] == '=')
            return EQUIV;
        if(data[0] == '(')
            return LEFTSK;
        if(data[0] == ')')
            return RIGHTSK;
        return UNKNOWN;
    }
    if(strlen(data) == 2)
    {
        if(data[0] == '-' && data[1] == '>')
            return IMPL;
        if(data[0] == '+' && data[1] == '>')
            return COMPL;
        if(data[0] == '<' && data[1] == '>')
            return SUMMOD2;
        return UNKNOWN;
    }
    return UNKNOWN;
}

status_code get_next(char* str, int index, token* result, int* len)
{
    if(str == NULL || index >= strlen(str))
        return INVALID_PARAMETER;
    char* tok = (char*) malloc(sizeof(char) * 3);
    if(!tok)
        return ALLOC_ERROR;
    if(str[index] == '-' || str[index] == '+' || str[index] == '<')
    {
        tok[0] = str[index];
        tok[1] = str[index + 1];
        tok[2] = '\0'; 
        *len = 2;
    }
    else
    {
        tok[0] = str[index];
        tok[1] = '\0';
        *len = 1;
    }
    *result = get_token(tok);
    free(tok);
    if(*result == UNKNOWN)
        return INPUT_ERROR;
    return OK;
}

int get_priority(token tok)
{
    switch(tok)
    {
        case DISJ:
        case IMPL:
        case SUMMOD2:
        case EQUIV:
            return 1;
        case VEBB:
        case SHEFFER:
        case COMPL:
        case CONJ:
            return 2;
        case INV:
            return 3;
        default:
            return 0;
    }
}

bool is_binary_operation(token t)
{  
    return t != TMP && t != CNST && t != INV && t != LEFTSK && t != RIGHTSK && t != UNKNOWN;
}

bool are_neighb_valid(token a, token b)
{
    if((a == TMP || a == CNST) && (b == TMP || b == CNST))
        return false;
    if((a == LEFTSK || a == RIGHTSK) && (b == LEFTSK || b == RIGHTSK) && (a != b))
        return false;
    if((is_binary_operation(a) || a == INV) && (b == RIGHTSK))
        return false;
    if(a == LEFTSK && (is_binary_operation(b)))
        return false;
    if((is_binary_operation(a) || a == INV) && (is_binary_operation(b)))
        return false;
    if((a == TMP || a == CNST) && b == LEFTSK)
        return false;
    if(a == RIGHTSK && (b == TMP || b == CNST))
        return false;
    return true;
}

status_code get_tokens(char* expr, token** res, int* res_cnt)
{
    status_code error = OK;
    int mx_size = 2;
    int size = 0;
    if(expr == NULL)
        return INVALID_PARAMETER;
    token* result = (token*) calloc(mx_size, sizeof(token));
    if(!result)
        return ALLOC_ERROR;
    int ind = 0;
    int ln = strlen(expr);
    while(ind < ln)
    {
        int len = 1;
        error = get_next(expr, ind, &(result[size++]), &len);

        if(error)
        {
            free(result);
            return error;
        }
        if(size == mx_size)
        {
            mx_size *= 2;
            token* copy_to = (token*) realloc(result, mx_size * sizeof(token));
            if(!copy_to)
            {
                free(result);
                return ALLOC_ERROR;
            }
            result = copy_to;
        }
        ind += len;
    }
    if(is_binary_operation(result[0]) || result[0] == RIGHTSK 
        || result[size - 1] == LEFTSK || is_binary_operation(result[size - 1]) || result[size - 1] == INV)
    {
        free(result);
        return INPUT_ERROR;
    }
    for(int i = 0; i < size - 1; i++)
    {
        if(!are_neighb_valid(result[i], result[i + 1]))
        {
            
            free(result);
            return INPUT_ERROR;
        }
    }
    *res = result;
    *res_cnt = size;
    return OK;
}

//

typedef struct Variables
{
    int size;
    char vars[26];
    unsigned int mask;//32 bits
} Variables;

void variables_setzero(Variables* vars)
{
    vars->size = 0;
    (*vars).vars[0] = '\0';
    vars->mask = 0;
}

int get_var_val(Variables v, char var)
{
    int found = -1;
    for(int i = 0; i < v.size; i++)
    {
        if(v.vars[i] == toupper(var))
            found = i;
    }
    if(found == -1)
        return -1;
    return (v.mask & (1 << found)) != 0;
}

void print_vars_vals(Variables v, char sep[], bool need_vars, FILE* whereto)
{
    if(!whereto)
        return;
    if(need_vars) fprintf(whereto, "%s%s",v.vars, sep);
    for(int i = 0; i < v.size; i++)
    {
        fprintf(whereto, "%d", get_var_val(v, v.vars[i]));
    }
}

void next_perm(Variables* v)
{
    unsigned int overflow = 1 << v->size;
    v->mask += 1;
    if(v->mask == overflow)
        v->mask = 0;
}

void add_var(Variables* v, char c)
{
    c = toupper(c);
    if(v == NULL)
        return;
    for(int i = 0; i < v->size; i++)
    {
        if(v->vars[i] == c)
            return;
    }
    (*v).vars[v->size] = c;
    v->size += 1;
    (*v).vars[v->size] = '\0';
}

typedef struct Tree_node
{
    token t;
    char tmp_name;
    struct Tree_node* right;
    struct Tree_node* left;
} Tree_node;

typedef struct St_node
{
    Tree_node* data;
    struct St_node* next;
} St_node;

typedef struct Stack
{
    St_node* top;
    int size;
} Stack;

void tree_node_destroy(Tree_node* node)
{
    if(node == NULL)
        return;
    if(node->right != NULL)
    {
        tree_node_destroy(node->right);
        free(node->right);
        node->right = NULL;
    }
    if(node->left != NULL)
    {
        tree_node_destroy(node->left);
        free(node->left);
        node->left = NULL;
    }
}

status_code tree_node_create(Tree_node** node, token t, char tmp_name)
{
    if(node == NULL)
        return INVALID_PARAMETER;
    *node = (Tree_node*) calloc(1, sizeof(Tree_node));
    if(!(*node))
        return ALLOC_ERROR;
    (*node)->tmp_name = tmp_name;
    (*node)->t = t;
    (*node)->left = NULL;
    (*node)->right = NULL;
    return OK;
}

status_code stack_create(Stack* st)
{
    if(st == NULL)
        return INVALID_PARAMETER;

    st->top = NULL;
    st->size = 0;
    return OK;
}
status_code stack_destroy(Stack* st)
{
    if(st == NULL)
        return INVALID_PARAMETER;
    St_node* node = st->top;
    if(st->top == NULL)
        return OK;
    while(node->next != NULL)
    {
        St_node* cur = node;
        tree_node_destroy(cur->data);
        node = node->next;
        free(cur);
        cur = NULL;
    }
    free(node);
    st->top = NULL;
    st->size = 0;
    return OK;
}
status_code stack_push(Stack* st, Tree_node* node)
{
    if(st == NULL)
        return INVALID_PARAMETER;
    St_node* stn = (St_node*) calloc(1, sizeof(St_node));
    if(!node)
        return ALLOC_ERROR;
    st->size++;
    stn->data = node;
    stn->next = st->top;
    st->top = stn;
    return OK;
}
status_code stack_pop(Stack* st, Tree_node** node)
{
    if(st == NULL || st->size == 0)
        return INVALID_PARAMETER;
    St_node* to_destroy = st->top;
    if(st->top == NULL)
        return INVALID_PARAMETER;
    if(node != NULL)
        *node = st->top->data;
    st->size--;
    st->top = st->top->next;
    free(to_destroy);
    to_destroy = NULL;
    return OK;
}

void destroy_stack(Stack* st)
{
    for(int i = 0; i < st->size; i++)
    {
        St_node* node;
        node = st->top;
        tree_node_destroy(node->data);
        st->top = node->next;
        node->data = NULL;
        free(node);
        node = NULL;
    }
}

status_code createStack(Stack* res)
{
    Stack* r = (Stack*) calloc(1, sizeof(Stack));
    if(!r)
        return ALLOC_ERROR;
    *res = *r;
    return OK;
}

// Функция для создания нового узла
status_code st_node_create(Tree_node* data, St_node* nd) {
    St_node* node = (St_node*)malloc(sizeof(St_node));
    if(!node)
        return ALLOC_ERROR;
    node->data = data;
    node->next = NULL;
    *nd = *node;
    return OK;
}

status_code create_expression_tree(char* expr, Tree_node** root)
{
    status_code error = OK;
    Stack opers_st;
    Stack letters_st;

    createStack(&opers_st);
    createStack(&letters_st);

    token* tokens;
    Variables vars;
    Tree_node* expr_tree;

    variables_setzero(&vars);
    int size;
    error = get_tokens(expr, &tokens, &size);
    if(error)
        return INPUT_ERROR;
    for(int i = 0; i < strlen(expr); )
    {
        if(error)
            break;
        int len = 0;
        token t;
        get_next(expr, i, &t, &len);
        if(t == TMP)
            add_var(&vars, expr[i]);

        if(t == TMP || t == CNST)
        {
            Tree_node* cur = NULL;
            error = error ? error : tree_node_create(&cur, t, expr[i]);
            error = error ? error : stack_push(&letters_st, cur);
        }
        else if(is_binary_operation(t) || t == INV)
        {
            Tree_node* cur = NULL;
            cur = opers_st.top == NULL ? NULL : opers_st.top->data;
            token t2 = cur != NULL ? cur->t : UNKNOWN;
            int prior = get_priority(t);
            int prior2 = get_priority(t2);
            while(error == 0 && prior2 >= prior && t != INV)
            {
                int cntarg = (t2 == INV) ? 1 : 2;
                error = letters_st.size >= cntarg ? OK : INPUT_ERROR;
                if(cntarg == 2)
                    error = error ? error : stack_pop(&letters_st, &cur->right);
                error = error ? error : stack_pop(&letters_st, &cur->left);
                error = error ? error : stack_push(&letters_st, cur);
                error = error ? error : stack_pop(&opers_st, NULL);
                cur = opers_st.top == NULL ? NULL : opers_st.top->data;
                t2 = cur != NULL ? cur->t : UNKNOWN;
                prior2 = get_priority(t2);
            }
            error = error ? error : tree_node_create(&cur, t, expr[i]);
            error = error ? error : stack_push(&opers_st, cur);
        }
        else if (t == RIGHTSK)
        {
            Tree_node* cur = NULL;
            cur = opers_st.top == NULL ? NULL : opers_st.top->data;
            error = cur != NULL ? OK : INPUT_ERROR;
            while(error == 0 && cur->t != LEFTSK)
            {
                int cntarg = (t == INV) ? 1 : 2;
                error = letters_st.size >= cntarg ? OK : INPUT_ERROR;
                if(cntarg == 2)
                    error = error ? error : stack_pop(&letters_st, &cur->right);
                error = error ? error : stack_pop(&letters_st, &cur->left);
                error = error ? error : stack_push(&letters_st, cur);
                error = error ? error : stack_pop(&opers_st, NULL);
                cur = opers_st.top == NULL ? NULL : opers_st.top->data;
                error = error ? error : cur == NULL ? INPUT_ERROR : OK;
            }
            cur = NULL;
            error = error ? error : stack_pop(&opers_st, &cur);
            free(cur);
            cur = NULL;
        }
        else if (t == LEFTSK)
        {
            Tree_node* cur = NULL;
            error = error ? error : tree_node_create(&cur, t, expr[i]);
            error = error ? error : stack_push(&opers_st, cur);
        }
        i += len;
    }
    while(error == 0 && opers_st.size != 0)
    {
        Tree_node* cur = NULL;
        error = stack_pop(&opers_st, &cur);
        int cntarg = 0;
        if(error == 0)
            cntarg = (cur->t == INV) ? 1 : 2;
        error = error ? error : (cur->t != LEFTSK ? OK : INPUT_ERROR);
        error = error ? error : (letters_st.size >= cntarg ? OK : INPUT_ERROR);
        if(cntarg == 2)
            error = error ? error : stack_pop(&letters_st, &cur->right);
        error = error ? error : stack_pop(&letters_st, &cur->left);
        error = error ? error : stack_push(&letters_st, cur);
        if(error)
        {
            tree_node_destroy(cur);
            free(cur);
            cur = NULL;
        }
    }
    
    if(!error)
    {
        error = letters_st.size == 1 ? OK : INPUT_ERROR;
        Tree_node* cur = NULL;
        error = error ? error : stack_pop(&letters_st, &cur);
        *root = cur;
    }
    stack_destroy(&opers_st);
    stack_destroy(&letters_st);
    if(error)
    {
        *root = NULL;
        return error;
    }
    return OK;
}

status_code calculate_expr_tree(Tree_node* root, Variables vars, int* result)
{
    if(root == NULL)
        return INPUT_ERROR;
    status_code error = OK;
    if(root->t == CNST)
    {
        if(root->tmp_name == '0')
            *result = 0;
        if(root->tmp_name == '1')
            *result = 1;
        return OK;
    }
    if(root->t == TMP)
    {
        int val = get_var_val(vars, root->tmp_name);
        if(val == 0)
            *result = 0;
        if(val == 1)
            *result = 1;
        return OK;
    }
    if(root->t == INV)
    {
        int res = 0;
        error = error ? error : calculate_expr_tree(root->left, vars, &res);
        if(error)
            return error;
        if(res == 0)
            *result = 1;
        else
            *result = 0;
        return OK;
    }
    else
    {
        int res1 = 0;
        int res2 = 0;
        error = error ? error : calculate_expr_tree(root->right, vars, &res1);
        error = error ? error : calculate_expr_tree(root->left, vars, &res2);
        if(error)
            return error;
        if(root->t == DISJ)
            *result = res1 || res2;
        if(root->t == CONJ)
            *result = res1 && res2;
        if(root->t == IMPL)
            *result = !res1 || res2;
        if(root->t == COMPL)
            *result = !res1 && res2;
        if(root->t == SUMMOD2)
            *result = res1 ^ res2;
        if(root->t == EQUIV)
            *result = res1 == res2;
        if(root->t == SHEFFER)
            *result = !(res1 && res2);
        if(root->t == VEBB)
            *result = !(res1 || res2);
        if(root->t == UNKNOWN)
            return INVALID_PARAMETER;
        return OK;

    }
}

status_code create_outpath(char** path, char* input_path)
{
    int generate_len = 10;
    if(input_path == NULL)
        return INVALID_PARAMETER;
    *path = (char*) malloc(sizeof(char) * (strlen(input_path) + generate_len + 1));
    if(*path == NULL)
        return ALLOC_ERROR;
    int last_slash = -1;
    for(int i = 0; i < strlen(input_path); i++)
    {
        if(input_path[i] == '\\')
            last_slash = i;
    }
    char* name = (char*)malloc((generate_len + 1) * sizeof(char));
    if(!name)
    {
        free(*path);
        *path = NULL;
        return ALLOC_ERROR;
    }
    srand(time(NULL));
    for(int i = 0; i < generate_len;)
    {
        int razbr = 'z' - '0' + 1;
        char found = (char) ('0' + (rand() % razbr));
        if(isdigit(found) || isalpha(found))
        {
            name[i] = found;
            i++;
        }
        name[generate_len] = '\0';
    }
    if(last_slash == -1)
    {
        strcat(*path, name);
    }
    else
    {
        for(int i = 0; i <= last_slash; i++)
        {
            (*path)[i] = input_path[i];
        }
        for(int i = last_slash + 1; i < last_slash + 1 + generate_len; i++)
             (*path)[i] = name[i - last_slash - 1];
    }
    (*path)[last_slash + 1 + generate_len] = '\0';
    return OK;
}

status_code fread_string(FILE* f, char** res)
{
    int MAX_INPUT_SIZE = 1024;
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

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        show_error(INPUT_ERROR);
        return INPUT_ERROR;
    }
    char* expr = NULL;
    //
    FILE* wheref = fopen(argv[1], "r");
    if(!wheref)
    {
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    status_code error = OK;
    error = fread_string(wheref, &expr);
    if(error)
    {
        fclose(wheref);
        show_error(INPUT_ERROR);
        return INPUT_ERROR;
    }
    fclose(wheref);
    //

    char* out;
    error = create_outpath(&out, argv[0]);
    if(error)
    {
        free(expr);
        free(out);
        show_error(error);
        return error;
    }
    FILE* fw = fopen(out, "w");
    if(!fw)
    {
        free(expr);
        free(out);
        show_error(FILE_ERROR);
        return FILE_ERROR; 
    }

    token* tokens;
    Variables vars;
    Tree_node* expr_tree;
    variables_setzero(&vars);
    int size;
    error = error ? error : get_tokens(expr, &tokens, &size);

    error = error ? error : create_expression_tree(expr, &expr_tree);
    int result = 0;
    error = error ? error : calculate_expr_tree(expr_tree, vars, &result);
    if(error)
    {
        fclose(fw);
        free(expr);
        free(out);
        show_error(error);
        return error;
    }
    for(int i = 0; i < strlen(expr); )
    {
        int len = 0;
        token t;
        get_next(expr, i, &t, &len);
        if(t == TMP)
            add_var(&vars, expr[i]);
        i += len;
    }
    if(vars.size == 0)
    {
        error = error ? error : calculate_expr_tree(expr_tree, vars, &result);
        if(!error){
            fprintf(fw, "Always constant:");
            fprintf(fw, "\t%d", result);
        }
    }
    else
    {
        error = error ? error : calculate_expr_tree(expr_tree, vars, &result);
        if(!error)
        {
            fprintf(fw, "%s\t%s\n", vars.vars, expr);
            print_vars_vals(vars, "\n", false, fw);
            fprintf(fw, "\t%d", result);
            fprintf(fw, "\n");
            next_perm(&vars);
            while(vars.mask != 0)
            {
                print_vars_vals(vars, "\n", false, fw);
                error = error ? error : calculate_expr_tree(expr_tree, vars, &result);
                if(!error)
                {
                    fprintf(fw, "\t%d", result);
                    next_perm(&vars);
                    fprintf(fw, "\n");
                }
            }
        }
    }
    if(error)
        show_error(error);
    free(tokens);
    tree_node_destroy(expr_tree);
    free(expr);
    free(out);
    fclose(fw);
    return error;
}