#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>

int bin_search_closest(int* b, int size, int number, int* result)
{
    if(size == 0 || b == NULL || *b == NULL)
        return INVALID_PARAMETER;

    int mid;
    int low = 0;
    int high = size;

    while (low < high) {
        mid = low + (high - low) / 2;

        if (number <= b[mid]) {
            high = mid;
        }
        else 
        {
            low = mid + 1;
        }
    }
    
    if(low >= size) {
       *result =  b[size - 1];
       return OK;
    }
    if(low < 0 || low == 0)
    {
        *result = b[0];
        return OK;
    }
    int right = b[low - 1];
    int left = b[low];
    if(abs(number - right) <= abs(number - left))
    {
        *result = right;
    }
    else
    {
        *result = left;
    }
    return OK;
}

int main() {

    int arr[5] = {1,3,5,7,10};
    int a;
    bin_search_closest(arr, 5, 0, &a);
    printf("%d\n", a);   
    bin_search_closest(arr, 5, 6, &a);
    printf("%d\n", a);  
    bin_search_closest(arr, 5, 7, &a);
    printf("%d\n", a);  
    bin_search_closest(arr, 5, 11, &a);
    printf("%d\n", a);  
    bin_search_closest(arr, 5, 8, &a);
    printf("%d\n", a);  
    bin_search_closest(arr, 5, 4, &a);
    printf("%d\n", a);  
    bin_search_closest(arr, 5, 1, &a);
    printf("%d\n", a);  
    return 0;
}