#include "pwd.h"

int pwd()
{
    char curr_dir[4500];
    if (getcwd(curr_dir, 4500) == NULL)
    {
        perror("PWD Error ");
        return 1;
    }
    printf("%s\n", curr_dir);
    return 0;
}