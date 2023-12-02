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
        obj = NULL;
    }

    va_end(argptr);
}

int show_error(status_code code)
{
    printf(function_base_errors[code]);
    return code;
}
//--------------------------------------------------------------------------------
//Just some more
typedef struct 
{
    unsigned int size;
    double* elems;
} Vector;

//Квадратная
typedef struct
{
    unsigned int size;
    double** elems;
} Matrix;

void print_matrix(Matrix* matr)
{
    for(int i = 0; i < matr->size; i++)
    {
        for(int j = 0; j < matr->size; j++)
        {
            printf("%lf ", matr->elems[i][j]);
        }
        printf("\n");
    }
}
void print_vectors(Vector** vecs, int cnt, int dim)
{
    if(vecs == NULL)
        return;
    for(int i = 0; i < cnt; i++)
    {
        printf("vector #%d:", i + 1);
        for(int j = 0; j < dim; j++)
        {
            if(vecs[i] == NULL || vecs[i]->elems == NULL)
                return;
            printf("%lf ", vecs[i]->elems[j]);
        }
        printf("\n");
    }
}
void matrix_free(Matrix* matrix, unsigned int i_max, bool dynamic)
{
    if(matrix == NULL)
        return;
    for(int i = 0; i < i_max; i++)
    {
        free(matrix->elems[i]);
    }
    free(matrix->elems);
    if(dynamic)
        free(matrix);
    matrix = NULL;
}

void vector_free(Vector* vector, bool dynamic)
{
    if(vector == NULL)
        return;
    free(vector->elems);
    if(dynamic)
        free(vector);
    vector = NULL;
}

status_code create_vector(unsigned int size, Vector* result)
{
    Vector v;
    if(size == 0)
        return INVALID_PARAMETER;
    v.size = size;
    v.elems = (double*) calloc(size, sizeof(double));
    if(v.elems == NULL)
        return ALLOC_ERROR;
    *result = v;
    return OK;
} 

status_code create_matrix(unsigned int size, Matrix* result)
{
    Matrix r;
    if(size == 0)
        return INVALID_PARAMETER;
    r.size = size;
    r.elems = (double**) calloc(size, sizeof(double*));
    if(r.elems == NULL)
    {
        matrix_free(&r, 0, false);
        return ALLOC_ERROR;
    }
    for(int i = 0; i < size; i++)
    {
        r.elems[i] = (double*) calloc(size, sizeof(double));
        if(r.elems[i] == NULL)
        {
            matrix_free(&r, i, false);
            return ALLOC_ERROR;
        }
    }
    *result = r;
    return OK;
}

status_code create_minor(Matrix* matrix, int i_excl, int j_excl, Matrix* result)
{
    if(matrix == NULL || i_excl >= matrix->size || i_excl < 0 || j_excl < 0 || j_excl >= matrix->size || matrix->size == 1)
        return INVALID_PARAMETER;
    status_code code = create_matrix(matrix->size - 1, result);
    if(code != OK)
        return code;
    
    int res_i = 0; 
    int res_j = 0;
    for(int i = 0; i < matrix->size; i++)
    {
        if(i == i_excl)
            continue;
        res_j = 0;
        for(int j = 0; j < matrix->size; j++)
        {
            if(j == j_excl)
                continue;
            result->elems[res_i][res_j] = matrix->elems[i][j];
            res_j++;
        }
        res_i++;
    }
    return OK;
}

bool matrix_correct(Matrix* matrix)
{
    if(matrix == NULL || matrix->size == 0 || matrix->elems == NULL)
        return false;
    for(int i = 0; i < matrix->size; i++)
    {
        if(matrix->elems[i] == NULL)
            return false;
    }
    return true;
}

bool vec_correct(Vector* vec)
{
    if(vec == NULL || vec->elems == NULL)
        return false;
    return true;
}
//через алг. дополнения
status_code det(Matrix* matrix, double* result, bool need_free)
{
    //print_matrix(matrix);
    status_code code = OK;
    if(result == NULL || !matrix_correct(matrix))
        return INVALID_PARAMETER;
    if(matrix->size == 1)
    {
        *result = matrix->elems[0][0];
        if(need_free)
        {
            matrix_free(matrix, matrix->size, false);
        }
        return OK;
    }
    double res = 0;
    Matrix minor;
    //j == 0
    double minorres = 0;
    for(int i = 0; i < matrix->size; i++)
    {
        code = create_minor(matrix, i, 0, &minor);
        code = code ? code : det(&minor, &minorres, true);
        if(code != OK)
        {
            if(need_free)
                matrix_free(matrix, matrix->size, false);
            return code;
        }
        double mn = ((i & 1) == 0 ? 1 : -1);
        res += matrix->elems[i][0] * minorres * mn;
    }
    *result = res;
    return OK;
}

status_code positive_determined(Matrix* matr, bool* res)
{
    status_code code = OK;
    if(!matrix_correct(matr))
        return INVALID_PARAMETER;
    double d = 0;
    Matrix m;
    for(int i = 0; i < matr->size; i++)
    {
        //Создание углового минора
        create_matrix(i + 1, &m);
        for(int j = 0; j <= i; j++)
        {
            for(int k = 0; k <= i; k++)
            {
                m.elems[j][k] = matr->elems[j][k];
            }
        }
        code = det(&m, &d, false);
        if(code != OK)
        {
            matrix_free(&m, m.size, false);
            return code;
        }
        if(!(d > 0))
        {
            *res = false;
            return OK;
        }
    }
    *res = true;
    return OK;
}

status_code matrix_mult_vec(Matrix* matr, Vector* vec, Vector* result)
{
    status_code code = OK;
    if(!matrix_correct(matr) || !vec_correct(vec) || matr->size != vec->size)
        return INVALID_PARAMETER;
    Vector r;
    code = create_vector(vec->size, &r);
    if(code != OK)
    {
        return code;
    }
    //to result
    for(int i = 0; i < r.size; i++)
    {
        double sum = 0;
        //sum
        for(int j = 0; j < r.size; j++)
        {
            sum += vec->elems[j] * matr->elems[i][j];
        }
        r.elems[i] = sum;
    }
    *result = r;
    return OK;
}

status_code dot(Vector* a, Vector* b, double* res)
{
    if(!vec_correct(a) || !vec_correct(b) || a->size != b->size)
        return INVALID_PARAMETER;
    *res = 0;
    for(int i = 0; i < a->size; i++)
    {
        *res += a->elems[i] * b->elems[i];
    }
    return OK;
}

status_code vec_norm_inf(Vector* vec, double* res)
{
    //dont need param
    if(!vec_correct(vec))
        return INVALID_PARAMETER;
    *res = 0;
    for(int i = 0; i < vec->size; i++)
    {
        *res = fmax(*res, fabs(vec->elems[i]));
    }
    return OK;
}

status_code vec_norm_p(Vector* vec, double param, double* res)
{
    if(!vec_correct(vec))
        return INVALID_PARAMETER;
    //param is double
    if(param < 1)
        return INVALID_PARAMETER;
    double sm = 0;
    for(int i = 0; i < vec->size; i++)
    {
        sm = sm + pow(fabs(vec->elems[i]), param);
    }
    *res = pow(sm, 1.0 / param);
    return OK;
}

status_code vec_norm_matrix(Vector* vec, Matrix* matr, double* res)
{
    status_code code = OK;
    if(!vec_correct(vec) || !matrix_correct(matr))
        return INVALID_PARAMETER;
    bool ok = true;
    code = positive_determined(matr, &ok);
    if(code != OK)
        return code;
    if(!ok)
        return INVALID_PARAMETER;
    Vector mult_res;
    create_vector(vec->size, &mult_res);
    code = matrix_mult_vec(matr, vec, &mult_res);
    code = code ? code : dot(vec, &mult_res, res);
    if(code != OK)
    {
        vector_free(&mult_res, false);
        return code;
    }
    if(*res < 0)
    {
        vector_free(&mult_res, false);
        return INVALID_PARAMETER;
    }
    *res = pow(*res, 0.5);
    return OK;
}

status_code get_max_vectors(Vector** allvecs_ptr, double* norms, int cnt, double eps, Vector*** result, int* rescount)
{
    if(cnt <= 0)
        return INVALID_PARAMETER;
    status_code code = OK;
    Vector** res = (Vector**) calloc(cnt, sizeof(Vector*));
    if(res == NULL)
        return ALLOC_ERROR;
    double mx = norms[0];
    for(int i = 0; i < cnt; i++)
        mx = fmax(mx, norms[i]);
    *rescount = 0;
    for(int i = 0; i < cnt; i++)
    {
        if(fabs(norms[i] - mx) < eps)
        {
            res[*rescount] = allvecs_ptr[i];
            (*rescount)++;
        }
    }
    *result = res;
    return OK;
}

status_code solve_function
(   unsigned int n, double eps,
    Vector*** norm_inf_list, int* cnt1, 
    Vector*** norm_p_list, int* cnt2, 
    Vector*** norm_matrix_list, int* cnt3,
    status_code (*norm1)(Vector*, double*),
    status_code (*norm2)(Vector*, double, double*), double par,
    status_code (*norm3)(Vector*, Matrix*, double*), Matrix* matr,
    unsigned int count, ...
)
    //Vector* ptr1, Vector* ptr2.....
{
    status_code code = OK;
    Vector** allvecs_ptr = (Vector**) calloc(count, sizeof(Vector*));
    double* norms = (double*) calloc(count, sizeof(double));
    if(norms == NULL || allvecs_ptr == NULL || count == 0)
        return ALLOC_ERROR;
   // for(int i = 0; i < )

    va_list argptr;
    va_start(argptr, count);

    for (int i = 0; i < count; i++)
    {
        allvecs_ptr[i] = va_arg(argptr, Vector*);
    }
    va_end(argptr);
    //первая норма
    for(int i = 0; i < count; i++)
    {
        code = norm1(allvecs_ptr[i], &(norms[i]));
        if(code)
        {
            free_all(2, allvecs_ptr, norms);
            free_all(3, *norm_inf_list, *norm_p_list, *norm_matrix_list);
            *norm_inf_list = NULL, *norm_p_list = NULL, *norm_matrix_list = NULL;
            return code;
        }
    }
    printf("\n");
    printf("norms for 1:\n");
    for(int i = 0; i < count; i++)
    {
        printf("%lf ", norms[i]);
    }
    printf("\n");
    code = get_max_vectors(allvecs_ptr, norms, count, eps, norm_inf_list, cnt1);
    if(code)
    {
        free_all(2, allvecs_ptr, norms);
        free_all(3, *norm_inf_list, *norm_p_list, *norm_matrix_list);
        *norm_inf_list = NULL, *norm_p_list = NULL, *norm_matrix_list = NULL;
        return code;
    }
    //Вторая норма
    for(int i = 0; i < count; i++)
    {
        code = norm2(allvecs_ptr[i], par, &(norms[i]));
        if(code)
        {
            free_all(2, allvecs_ptr, norms);
            free_all(3, *norm_inf_list, *norm_p_list, *norm_matrix_list);
            *norm_inf_list = NULL, *norm_p_list = NULL, *norm_matrix_list = NULL;
            return code;
        }
    }
    printf("norms for 2:\n");
    for(int i = 0; i < count; i++)
    {
        printf("%lf ", norms[i]);
    }
    printf("\n");
    code = get_max_vectors(allvecs_ptr, norms, count, eps, norm_p_list, cnt2);
    if(code)
    {
        free_all(2, allvecs_ptr, norms);
        free_all(3, *norm_inf_list, *norm_p_list, *norm_matrix_list);
        *norm_inf_list = NULL, *norm_p_list = NULL, *norm_matrix_list = NULL;
        return code;
    }
    printf("norms for 3:\n");
    
    //Третья норма
    for(int i = 0; i < count; i++)
    {
        code = norm3(allvecs_ptr[i], matr, &(norms[i]));
        if(code)
        {
            free_all(2, allvecs_ptr, norms);
            free_all(3, *norm_inf_list, *norm_p_list, *norm_matrix_list);
            *norm_inf_list = NULL, *norm_p_list = NULL, *norm_matrix_list = NULL;
            return code;
        }
    }
    for(int i = 0; i < count; i++)
    {
        printf("%lf ", norms[i]);
    }
    printf("\n");
    code = get_max_vectors(allvecs_ptr, norms, count, eps, norm_matrix_list, cnt3);
    free_all(2, allvecs_ptr, norms);
    return code;
}

const char usage[] = "<in_file_path> <out_file_path>\n";

int main(int argc, char** argv)
{
    status_code code = OK;
    int n = 2;
    Matrix matr;
    double dt = 0;
    Vector** v1 = NULL, **v2 = NULL, **v3 = NULL;
    int cnt1, cnt2, cnt3;
    create_matrix(n, &matr);
    //
    //3 -5
    //5 -1
    //filling matrix
    matr.elems[0][0] = 5.0; matr.elems[0][1] = 1.0;
    matr.elems[1][0] = -1.0; matr.elems[1][1] = 1.0;
    
    printf("Matrix A:\n");
    print_matrix(&matr);
    code = det(&matr, &dt, false);
    if(code != OK)
    {
        matrix_free(&matr, matr.size, false);
        return code;
    } 
    printf("det(A) = %lf\n", dt);
    //
    double p = 1.5;
    printf("Parameter for norm2 = %lf", p);
    //
    Vector vec1, vec2, vec3;
    code = create_vector(2, &vec1);
    code = code ? code : create_vector(2, &vec2);
    code = code ? code : create_vector(2, &vec3);
    //
    vec1.elems[0] = -10; vec1.elems[1] = 3;
    vec2.elems[0] = 10; vec2.elems[1] = 4;
    vec3.elems[0] = 5; vec3.elems[1] = 8;
    printf("\n");
    Vector* v[] = {&vec1, &vec2, &vec3};
    print_vectors(v, 3, 2);
    if(code != OK)
    {
        vector_free(&vec1, false);
        vector_free(&vec2, false);
        vector_free(&vec3, false);
        matrix_free(&matr, matr.size, false);
        show_error(code);
        return code;
    }
    //vec1 1 5
    //vec2 2 6
    //vec3 
    code = solve_function(n, 1e-7, &v1, &cnt1, &v2, &cnt2, &v3, &cnt3, vec_norm_inf, vec_norm_p, p, vec_norm_matrix, &matr,
        3, &vec1, &vec2, &vec3);
    printf("\n");
    if(code == OK)
    {
        printf("Results for norm1: \n");
        print_vectors(v1, cnt1, n);

        printf("Results for norm2: \n");
        print_vectors(v2, cnt2, n);

        printf("Results for norm3: \n");
        print_vectors(v3, cnt3, n);
    }
    else
        show_error(code);
    
    vector_free(&vec1, false);
    vector_free(&vec2, false);
    vector_free(&vec3, false);
    matrix_free(&matr, matr.size, false);
    free(v1);
    free(v2);
    free(v3);
    return code;
}