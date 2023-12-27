#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <math.h>


typedef enum status_code
{
    OK,
    INVALID_PARAMETER,
    INPUT_ERROR,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    ALLOC_ERROR,
    FILE_ERROR,
    UNKNOWN_ERROR,
    NOTHING,
} status_code;

static const char* function_base_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Input error\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error\n",
    "ERROR: File error\n",
    "ERROR: Unknown...\n",
    "ERROR: Empty...?\n"
};

int show_error(status_code code)
{
    printf("%s", function_base_errors[code]);
    return code;
}

typedef struct Node
{
    struct Node* parent;
    struct Node* left;
    struct Node* right;
    int count;
    char* value;
} Node;

void free_tree(Node* tree)
{
    if(tree == NULL)
        return;
    free(tree->value);
    tree->value = NULL;
    if(tree->left != NULL)
        free_tree(tree->left);
    if(tree->right != NULL)
        free_tree(tree->right);
    free(tree);
}

status_code prefix_node_create(char* val, Node** result)
{
    if(val == NULL || result == NULL)
        return INVALID_PARAMETER;
    *result = (Node*) calloc(1, sizeof(Node));
    if(!(*result))
        return ALLOC_ERROR;
    (*result)->count = 1;
    (*result)->value = val;
    return OK;
}

status_code tree_push(Node** tree, char* val)
{
    if(*tree == NULL)
        return prefix_node_create(val, tree);
    status_code error = OK;
    Node* par = *tree;
    
    int cmp = strcmp(par->value, val);
    if(cmp == 0)
    {
        par->count++;
        return OK;
    }
    else
    {
        Node* cur_par = NULL;
        do
        {
            cmp = strcmp(par->value, val);
            if(cmp == 0)
                break;
            cur_par =  cmp > 0 ? par->left : par->right;
            if(cur_par != NULL)
                par = cur_par;
        } while (cmp != 0 && cur_par != NULL);

        if(cur_par != NULL)
        {
            //в листе
            cur_par->count++;
            return OK;
        }
        else
        {
            //new
            Node* nd = NULL;
            error = prefix_node_create(val, &nd);
            if(error)
                return error;
            nd->parent = par;
            if(par == NULL)
            {
                return INVALID_PARAMETER;
            }
            else
            {
                cmp = strcmp(par->value, val);
                if(cmp > 0)
                    par->left = nd;
                else if (cmp < 0)
                    par->right = nd;
            }
        }
        return OK;
    }
}

//3
void get_val_node(Node* tree, char* val, Node** res)
{
    if(tree == NULL)
    {
        *res = NULL;
        return;
    }
    int cmp = strcmp(tree->value, val);
    if(cmp > 0)
        return get_val_node(tree->left, val, res);
    else if (cmp < 0)
        return get_val_node(tree->right, val, res);
    else
    {
        *res = tree;
    }
}

//1
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
//0
status_code fread_word(FILE* file, char** res)
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
    while((ch == '\n' || ch == ' ' || ch == '\t') && ch != EOF)
        ch = fgetc(file);

    while (ch != '\0' && ch != '\n' && ch != ' ' && ch != '\t' && ch != EOF)
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
//1
void print_tree_to_file(FILE* f, Node* tree)
{
    if(f == NULL || tree == NULL)
        return;
    for(int i = 0; i < tree->count; i++)
        fprintf(f, "%s\n", tree->value);

    print_tree_to_file(f, tree->left);
    print_tree_to_file(f, tree->right);
}
//2
int get_depth(Node* tree)
{
    if(tree == NULL)
        return 0;
    int l = get_depth(tree->left);
    int r = get_depth(tree->right);
    int mx = l > r ? l : r;
    return mx + 1;
}
//на вход mx нужно подать 0
void get_max_frequency(Node* tree, int* mx)
{
    if (tree == NULL) 
        return;

    if (tree->count > *mx) 
        *mx = tree->count;
    if (tree->left != NULL) 
        get_max_frequency(tree->left, mx);
    if (tree->right != NULL) 
        get_max_frequency(tree->right, mx);
}

void print_nodes_fixed_frequency(Node* tree, int freq, int* count_written)
{
    if (!tree) 
        return;

    if (tree->count == freq)
    {
        if((*count_written) > 0)
        {
            (*count_written)--;
            printf("%20s|%10d\n", tree->value, tree->count);
        }
    }
    if(tree->left)
        print_nodes_fixed_frequency(tree->left, freq, count_written);

    if(tree->right)
        print_nodes_fixed_frequency(tree->right, freq, count_written);
}
//4
void print_most_quantity(Node* root, int N)
{
    if(N <= 0)
        return;
    int mx = 0;
    get_max_frequency(root, &mx);
    while (N > 0 && mx > 0)
    {
        print_nodes_fixed_frequency(root, mx, &N);
        mx--;
    }
    if (mx == 0 && N > 0)
        printf("All words were printed. You entered a too big N.");
    printf("\n");
}

//mxlen = -10000
//5
void get_longest(Node* root, int* mx_len, Node** res)
{
    if(root == NULL)
    {    
        *res = NULL;
        return;
    }
    int cur = strlen(root->value);
    if(cur > *mx_len)
    {
        *mx_len = cur;
        *res = root;
    }   
    if(root->left != NULL) get_longest(root->left, mx_len, res);
    if(root->right != NULL) get_longest(root->right, mx_len, res);
}
//mxlen = 1000000000000000
//6
void get_shortest(Node* root, int* mx_len, Node** res)
{
    if(root == NULL)
    {    
        *res = NULL;
        return;
    }
    int cur = strlen(root->value);
    if(cur < *mx_len)
    {
        *mx_len = cur;
        *res = root;
    }   
    if(root->left != NULL) get_shortest(root->left, mx_len, res);
    if(root->right != NULL) get_shortest(root->right, mx_len, res);
}


void usage()
{
    printf("Choose command:\n");
    printf("1) Save tree to file and load from file\n");
    printf("2) Get max depth\n");
    printf("3) Count of the word in text\n");
    printf("4) Print most frequent words (write count to display too)\n");
    printf("5) Find a shortest word\n");
    printf("6) Find a longest word\n");
    printf("7) Close\n");
}

void unknown()
{
    printf("Commands params are wrong. Please read usage...\n");
}

status_code dialogue(Node** root)
{
    status_code error = OK;
    usage();
    while(!error)
    {
        int cmd = 0;
        int res = scanf("%d", &cmd);
        if(res != 1 || !(cmd >= 1 && cmd <= 7))
        {
            unknown();
            break;
        }
        if(cmd == 1)
        {
            FILE* f = tmpfile();
            if (f == NULL) {
                return FILE_ERROR;
            }

            print_tree_to_file(f, *root);
            fseek(f, 0, SEEK_SET);

            free_tree(*root);
            *root = NULL;

            char* tmp = NULL;
            while(!error && ((error = fread_line(f, &tmp)) == OK))
            {
                if(!strcmp(tmp,"\0"))
                    break;
                Node* nd = NULL;
                error = error ? error : tree_push(root, tmp);
                tmp = NULL;
            }
            tmp = NULL;
            fclose(f);
        }
        else if (cmd == 2)
        {
            printf("Depth: %d\n", get_depth(*root));
        }
        else if (cmd == 3)
        {
            char word[1024];
            scanf("%s", word);
            Node* nd = NULL;
            get_val_node(*root, word, &nd);
            if(nd == NULL)
            {
                printf("Nothing found...");
            }
            else
            {
                printf("Found %d times\n", nd->count);
            }
        }
        else if (cmd == 4)
        {
            int num = 0;
            int res = scanf("%d", &num);
            if(res != 1 || num <= 0)
            {
                printf("N should be greater than 0");
                continue;
            }
            print_most_quantity(*root, num);
            fflush(stdin);
        }
        else if (cmd == 5)
        {
            Node* shr = NULL;
            int mx = INT_MAX;
            get_shortest(*root, &mx, &shr);
            if (shr == NULL) {
                printf("tree is empty...\n");
            }
            else 
            {
                printf("Shortest word: %s\n", shr->value);
            }
        }
        else if (cmd == 6)
        {
            Node* lng = NULL;
            int mx = -1;
            get_longest(*root, &mx, &lng);
            if (lng == NULL) {
                printf("tree is empty...\n");
            }
            else 
            {
                printf("Longest word: %s\n", lng->value);
            }
        }
        else if (cmd == 7)
        {
            break;
        }
        fflush(stdin);
    }
    return OK;
}

int main(int argc, char** argv)
{
    // argc = 2;
    // argv[1] = "df";
    if(argc != 2)
    {
        printf("use: <input_file>\n");
        return INPUT_ERROR;
    }
    status_code error = OK;

    FILE* f = fopen(argv[1], "r");
    if(!f)
    {
        show_error(FILE_ERROR);
        return FILE_ERROR;
    }
    Node* tree = NULL;
    char* tmp = NULL;
    while(!error && ((error = fread_word(f, &tmp)) == OK))
    {
        if(!strcmp(tmp,"\0"))
            break;
        Node* nd = NULL;
        error = error ? error : tree_push(&tree, tmp);
        tmp = NULL;
    }
    tmp = NULL;
    
    error = error ? error : dialogue(&tree);

    fclose(f);
    free_tree(tree);
    if(error)
        show_error(error);
    return error;
}