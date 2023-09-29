#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

enum functions_status_codes
{
    OK = 0,
    INVALID_PARAMETER,
    OVERFLOW,
    DIVISION_BY_ZERO,
    UNKNOWN_ERROR
};

static const char* function_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter\n",
    "ERROR: Overflow\n",
    "ERROR: Division by zero\n",
    "ERROR: Unknown\n"
};

functions_status_codes str_to_int(char* str, int* result)
{
    if (str == NULL)
    {
        *result = 0;
        return INVALID_PARAMETER;
    }
    if (str == "-" || str == "+")
    {
        *result = 0;
        return INVALID_PARAMETER;
    }

    int count = strlen(str);
    //check if is number
    bool check_first = (isdigit(str[0]) || str[0] == '-' || str[0] == '+');
    for (int i = 0; i < count; i++)
    {
        if (!check_first || (i != 0 && !isdigit(str[i])))
        {
            *result = 0;
            return INVALID_PARAMETER;
        }
    }

    //-[INT_MAX;INT_MAX]
    *result = 0;
    int digits = 0;
    bool is_counting_zeros = false;
    int multiplier = 1;

    if (str[0] == '-')
    {
        multiplier = -1;
    }
    if (str[0] == '+' || str[0] == '-')
    {
        str[0] = '0';
    }
    for (int i = 0; i < count; i++)
    {
        if (isdigit(str[i]) && str[i] != '0')
        {
            is_counting_zeros = true;
            digits++;
        }
        else if(str[i] == '0' && is_counting_zeros)
        {
            digits++;
        }
        if (is_counting_zeros)
        {
            if (digits == 1)
            {
                *result = multiplier * (str[i] - '0');
            }
            else
            {
                if (digits < 10)
                {
                    *result = *result * 10 + (multiplier) * (str[i] - '0');
                }

                if (digits == 10)
                {
                    if (abs(*result) > INT_MAX / 10)
                    {
                        *result = 0;
                        return OVERFLOW;
                    }
                    else if (abs(*result) == INT_MAX / 10)
                    {
                        //INT_MAX mod 10 == 7
                        //INT_MIN mod 10 == 8
                        if ( ((str[i] - '0') <= 7 && multiplier == 1) || ((str[i] - '0') <= 8 && multiplier == -1) )
                        {
                            *result = *result * 10 + (multiplier) * (str[i] - '0');
                        }
                        else
                        {
                            *result = 0;
                            return OVERFLOW;
                        }
                    }
                    else
                    {
                        *result = *result * 10 + (multiplier) * (str[i] - '0');
                    }
                }

                if (digits > 10)
                {
                    *result = 0;
                    return OVERFLOW;
                }
            }
        }

    }
    if (digits == 0)
    {
        *result = 0;
    }
    return OK;
}

int INT_max = 2147483647;
int main(){

    int num = 0;

    for(int i = 0; i < 100; i++){
        char s[50];
        scanf("%s", &s);
        functions_status_codes x = str_to_int(s, &num);
        printf("%d %s",num, function_errors[x]);
    }

    //int num3 = str_to_int("10000000000000000", &num);

    //int num4 = str_to_int("-000000000000067678", &num);

    //int num5 = str_to_int("2.4123", &num);

    //int num6 = str_to_int("234.sd32", &num);

}