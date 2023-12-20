#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main()
{
    pid_t pid = getpid();
    pid_t parent_pid = getppid();
    pid_t group_pid = getpgrp();

    uid_t uid = getuid();
    uid_t euid = geteuid();

    gid_t gid = getgid();
    gid_t egid = getegid();

    printf("Process ID: %d\n", pid);
    printf("Parent process ID: %d\n", parent_pid);
    printf("Group process ID: %d\n", group_pid);
    printf("Real user ID: %d\n", uid);
    printf("Effective user ID: %d\n", euid);
    printf("Real group ID: %d\n", gid);
    printf("Effective group ID: %d\n", egid);
}