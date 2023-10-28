#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_BUFF_SIZE 1024

#define BYTE_SIZE_WRITE 11
#define BYTE_SIZE_READ 4
#define SEEK_SET_SHIFT 3

static const char* usage = "Usage: <command> <file> [<hex> - for command mask]\n";

typedef enum
{
    OK = 0,
    INVALID_PARAMETER,
    MY_OVERFLOW,
    DIVISION_BY_ZERO,
    ALLOC_ERROR,
    FILE_ERROR,
    UNKNOWN_INPUT,
    UNKNOWN_ERROR,
    //
    INVALID_PIN,
    WRONG_PIN,
    INVALID_LOGIN,
    LOGIN_EXISTS,
    SANCTIONS,

} status_code;

static const char* print_errors[] =
{
    "OK\n",
    "ERROR: Invalid parameter. Try again\n",
    "ERROR: OVERFLOW\n",
    "ERROR: Division by zero\n",
    "ERROR: Alloc error. Try another time\n",
    "ERROR: File error\n",
    "ERROR: Unknown input. Try again\n",
    "ERROR: Unknown...\n",
    //
    "ERROR: Invalid pin. Pin must be in range [0,100000]. Try again\n",
    "ERROR: Wrong pin. Try again\n",
    "ERROR: Invalid login. Login length must not be more than 6 symbols (digits or english letters). Try again\n",
    "ERROR: Login does not exist. Try again\n",
    "ERROR: You are not allowed to make more requests in one session. Try another time\n",
};
typedef enum
{
    LOGGED_IN,
    MAIN_MENU,
} menu_status;

typedef enum 
{
    EXIT,
    LOG_OUT,
    LOG_IN,
    REGISTER,
    TIME,
    DATE,
    HOWMUCH
} action;

bool is_valid_pin(int pin)
{
    return (pin >= 0 && pin <= 100000);
}
bool is_valid_login(char* login)
{
    if(login == NULL)
        return false;
    if(strlen(login) > 6)
        return false;
    for(int i = 0; i < strlen(login); i++)
    {
        if (!(isalpha(login[i]) || isdigit(login[i])))
            return false;
    }
    return true;
}

status_code get_data()
{




}

int main(int argc, char** argv)
{




}