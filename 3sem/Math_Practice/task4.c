#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define ull unsigned long long int
#define ll long long int

enum functions_status_codes
{
    OK = 0,
    INVALID_PARAMETER,
    OVERFLOW,
    DIVISION_BY_ZERO,
    UNKNOWN_ERROR,
    RESERVED
};

static const char* usage = "Usage:\n -<flag> <number> or\n /<flag> <number>\n flags: -h, -p, -s, -e, -a, -f\n ";

static const char* input_errors[] =
{
    "ERROR: Wrong count of parameters\n",
    "ERROR: Unknown flag\n",
    "ERROR: Unknown input\n"
};

int main(int argc, char** argv)
{
    
}