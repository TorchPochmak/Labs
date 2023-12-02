#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct
{
    int id;
} rangd;

int main()
{
    int i = 0;
    while(i++ < 5)
    {
        rangd x;
        printf("%d\n", x.id);
        x.id = 5;
    }
    getchar();

    // printf("HELLO");
    // char* a = NULL;
    // char* b = NULL;
    // char* c = "ab";
    // char* d = "abc";
    // char* e = "abd";
    // printf("%d\n", strcmp(c,d));
    // printf("%d\n", strcmp(d,e));
    // printf("%d\n", strcmp(c,c));

    // // //strcmp - NULL не понимает и падает!
    // // printf("%d\n", strcmp(a,b));
    // // printf("%d\n", strcmp(a,c));
    // // printf("%d\n", strcmp(a,a));
    // void* f = "sada";
    // printf("%d", (int*)f);
    // printf("OK");

    // char x[50] = "3432";
    // printf("%d", strlen(x));
}