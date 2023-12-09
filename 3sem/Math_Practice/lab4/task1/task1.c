#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello\n");
    char* g = (char*) malloc(sizeof(char));
    free(g);
}