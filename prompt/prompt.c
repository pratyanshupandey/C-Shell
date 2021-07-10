#include "prompt.h"
#include "../headers.h"

int prompt(char initial_dir[], int length, int ex_code)
{
    char hostname[100];
    if (gethostname(hostname, 100) == -1)
    {
        perror("gethostname ");
        return 1;
    }
    char *loginid = getlogin();
    if (loginid == NULL)
    {
        perror("loginid ");
        return 1;
    }
    char curr_dir[4500];
    if (getcwd(curr_dir, 4500) == NULL)
    {
        perror("Current Directory Error ");
        return 1;
    }
    int is_prefix = 0;
    if (strncmp(initial_dir, curr_dir, length) == 0)
        is_prefix = length;
    printf("%s", ex_code == 0 ? ":')<" : ":'(<");
    printf("%s@%s:%c%s> ", loginid, hostname, is_prefix ? '~' : '\0', curr_dir + is_prefix);
    return 0;
}
