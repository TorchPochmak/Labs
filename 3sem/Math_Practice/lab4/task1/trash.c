#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list
{
    int f;
} list;


void change(list* x)
{
    x->f = 4;
}
int main()
{
    list* x = (list*) calloc(1, sizeof(list));
    list u = *x;
    printf("%p %d\n", x, x->f);
    printf("%p %d\n", &u, u.f);
    change(&u);
    printf("%p %d\n", x, x->f);
    printf("%p %d\n", &u, u.f);

    free(&u);
    getchar();

}