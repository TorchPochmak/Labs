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
    //res = "foo1 after";
    res[1] = 56;
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

typedef struct A
{
    int id;
} A;

void getA(A* result)
{
    A ff;
    ff.id = 10;
    *result = ff;
}
int main()
{
    char* a = NULL;
    printf("%d", strlen(a));
    printf("\n");
}


