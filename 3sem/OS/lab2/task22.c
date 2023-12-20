#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	fork();
    printf("Hi");
    fork();
    printf("Hi");
}
