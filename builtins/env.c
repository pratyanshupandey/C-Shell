#include "env.h"

int setval(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    if (arg_num < 2)
    {
        printf("shell: too less arguments\n");
        return 1;
    }
    else if (arg_num > 3)
    {
        printf("shell: too many arguments\n");
        return 1;
    }
    if (arguments[2] == NULL)
        arguments[2] = "";
    if (setenv(arguments[1], arguments[2], 1) == -1)
    {
        perror("setenv");
        return 1;
    }
    return 0;
}

int unsetval(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    if (arg_num < 2)
    {
        printf("shell: too less arguments\n");
        return 1;
    }
    else if (arg_num > 2)
    {
        printf("shell: too many arguments\n");
        return 1;
    }

    if (unsetenv(arguments[1]) == -1)
    {
        perror("unsetenv");
        return 1;
    }
    return 0;
}