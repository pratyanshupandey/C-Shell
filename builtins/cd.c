#include "../headers.h"
#include "cd.h"

extern char prev_dir[4500];

int cd(char *arguments[], char initial_dir[], int ini_dir_length)
{
    int arg_num = no_of_arg(arguments);
    if (arg_num == 1)
        return 0;
    if (arg_num > 2)
    {
        printf("shell: too many arguments\n");
        return 1;
    }
    if (arguments[1][0] == '~')
    {
        strcat(initial_dir, arguments[1] + 1);
        arguments[1] = initial_dir;
    }
    else if (arguments[1][0] == '-')
    {
        printf("%s\n", prev_dir);
        arguments[1] = prev_dir;
    }

    char temp[4500];
    if (getcwd(temp, 4500) == NULL)
    {
        perror("getcwd ");
        return 1;
    }
    if (chdir(arguments[1]) == -1)
    {
        perror("changing directory ");
        initial_dir[ini_dir_length] = '\0';
        return 1;
    }
    initial_dir[ini_dir_length] = '\0';
    strcpy(prev_dir, temp);
    return 0;
}