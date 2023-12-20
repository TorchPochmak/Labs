#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	int pid;
	pid = fork();
	printf("%d\n", pid);
}
