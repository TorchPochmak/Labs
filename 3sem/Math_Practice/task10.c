#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

int main() {
    long double a = exp(1.0);
    printf("%Lf", a);
    printf("\n%.9Lf", exp(1.0));

    
    getchar();
    return 0;
}