#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_SIZE 512
#define USE_UNIX_SOCKETS
typedef enum status_code 
{
    OK,
    INVALID_PARAMETER,
    ALLOC_ERROR,
    FILE_ERROR,
    DIRECTORY_ERROR
} status_code;

void print_error(status_code code)
{
    switch(code)
    {
        case OK:
        {
            break;
        }
        case INVALID_PARAMETER:
        {
            printf("ERROR: Invalid Parameter...\n");
            break;
        }
        case ALLOC_ERROR:
        {
            printf("ERROR: Alloc error...\n");
            break;
        }
        case FILE_ERROR:
        {
            printf("ERROR: File error...\n");
            break;
        }
        case DIRECTORY_ERROR:
        {
            printf("ERROR: Directory error...\n");
            break;
        }
        default:
        {
            printf("ERROR: Unknown...\n");
            break;
        }
    }
}

status_code print_filetype(char* filepath)
{
    if(!filepath)
        return INVALID_PARAMETER;
    struct stat sb;
    if(stat(filepath, &sb) == -1)
        return FILE_ERROR;
    int n;
    switch (sb.st_mode & __S_IFMT) {
        case __S_IFBLK:  printf("(block device)\n");            break;
        case __S_IFCHR:  printf("(character device)\n");        break;
        case __S_IFDIR:  printf("(directory)\n");               break;
        case __S_IFIFO:  printf("(FIFO/pipe)\n");               break;
        case __S_IFLNK:  printf("(symlink)\n");                 break;
        case __S_IFREG:  printf("(regular file)\n");            break;
        case __S_IFSOCK: printf("(socket)\n");                  break;
        default:       printf("(unknown?)\n");                  break;
    }
    return OK;
}

status_code ls(char* dirpath)
{
    char buffer[BUFSIZ];
    DIR* directory = opendir(dirpath);
    if(!directory)
        return DIRECTORY_ERROR;
    
    status_code error = OK;
    struct dirent* dir;
    struct stat stats;
    printf("%s\n", dirpath);

    while((dir = readdir(directory)) != NULL)
    {
        snprintf(buffer, sizeof(buffer), "%s/%s", dirpath, dir->d_name);
        printf(" %s", dir->d_name);
        if((error = print_filetype(buffer)) != OK)
        {
            closedir(directory);
            return FILE_ERROR;
        }
    }
    closedir(directory);
    return OK;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: <dirpath1> <dirpath2> <dirpath3> ...\n");
        return INVALID_PARAMETER;
    }
    status_code error = OK;
    for (int i = 1; i < argc; i++) {
        error = ls(argv[i]);
        if(error)
            break;
    }
    print_error(error);
    return error;
}