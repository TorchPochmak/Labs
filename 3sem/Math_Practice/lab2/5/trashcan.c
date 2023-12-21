#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>


char* dump_memo(void* input, int size) {
    char* res = (char*)malloc(sizeof(char) * size * 9);
    if (!res)
        return NULL;
    int sz = 0;
    char* pt = (char*) input;
    for (int i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            char bit = (((*(pt+i)) >> j) & 1) == 0 ? '0' : '1';
            res[sz++] = bit;
        }
        res[sz++] = ' ';
    }
    res[sz] = '\0';
    return res;
}

int main()
{
    unsigned char* dmp =  dump_memo((void*) &a, sizeof(int));
    printf("%s", dmp);

}