#include "../headers.h"
#include "echo.h"

int echo(char *arguments[])
{
    int i = 1;
    while (arguments[i] != NULL)
    {
        printf("%s ", arguments[i]);
        i++;
    }
    printf("\n");
    return 0;
}