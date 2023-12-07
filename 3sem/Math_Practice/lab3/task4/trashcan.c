#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

//does not work
void foo1(char* res)
{
    res = "foo1 after";
}
//works
void foo2(char** res)
{
    *res = "foo2 after";
}
//does not work
void foo3(char** res)
{
    char* ok = "foo3 after";
    char** ok_ptr = &ok;
    res = ok_ptr;
}
//works
void foo4(char** res)
{
    char* ok = "foo4 after";
    char** ok_ptr = &ok;
    *res = *ok_ptr;
}
//does not work
void foo5(char** res)
{
    char* ok = "foo5 after";
    res = &ok;
}
//works, but not for the strings (there's no '\0')
void foo6(char** res)
{
    char ok = 'h';

    *res = &ok;
}
//works, but not for the strings (there's no '\0')
void foo61(char** res)
{
    char* ok = "689";

    *res = &ok;
}
//works, but it's not the string anymore
void foo7(int** res)
{
    free(*res);
    *res = NULL;
    int ok = 5;
    *res = &ok;
}
int main()
{
    int i = 2;
    char* res = "foo1 before";
    printf("%s\n", res);
    foo1(res);
    printf("%s\n", res);
    printf("\n");
    //
    res = "foo2 before";
    printf("%s\n", res);
    foo2(&res);
    printf("%s\n", res);
    printf("\n");
    //
    res = "foo3 before";
    printf("%s\n", res);
    foo3(&res);
    printf("%s\n", res);
    printf("\n");
    //
    res = "foo4 before";
    printf("%s\n", res);
    foo4(&res);
    printf("%s\n", res);
    printf("\n");
    //
    res = "foo5 before";
    printf("%s\n", res);
    foo5(&res);
    printf("%s\n", res);
    printf("\n");
    //
    res = "foo6 before";
    printf("%s\n", res);
    foo6(&res);
    //printf("%d", strlen(res));
    printf("%s\n", res);
    printf("\n");
    //
    res = "foo6 before with ints";
    int* a = (int*) calloc(3, sizeof(int));
    printf("%d\n", a[1]);
    foo7(&a);
    printf("%d\n", a[1]);
    printf("\n");
}


