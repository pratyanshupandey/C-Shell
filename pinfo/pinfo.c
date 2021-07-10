#include "pinfo.h"

int pinfo(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    pid_t pid = 0;
    if (arg_num == 1)
        pid = getpid();
    else
    {
        int len = strlen(arguments[1]);
        for (int i = 0; i < len; i++)
            pid += (int)(arguments[1][i] - '0') * pow(10, len - 1 - i);
    }

    char path[30];
    sprintf(path, "/proc/%d/stat", pid);
    FILE *o_file = fopen(path, "r");
    if (o_file == NULL)
    {
        perror("proc opening error ");
        return 1;
    }
    char left[30], status;
    fscanf(o_file, "%d %s %c", &pid, left, &status);
    printf("pid -- %d\n", pid);
    printf("Process Status -- %c\n", status);
    if (fclose(o_file) != 0)
    {
        perror("closing /proc/pid/stat ");
        return 1;
    }
    sprintf(path, "/proc/%d/statm", pid);
    o_file = fopen(path, "r");
    if (o_file == NULL)
    {
        perror("proc opening error ");
        return 1;
    }
    int memory;
    fscanf(o_file, "%d", &memory);
    printf("memory -- %d\n", memory * 4096);
    if (fclose(o_file) != 0)
    {
        perror("closing /proc/pid/stat ");
        return 1;
    }
    unsigned long int size = 1000;
    char *line = (char *)malloc(size);
    sprintf(path, "/proc/%d/exe", pid);
    int len;
    if ((len = readlink(path, line, size)) == -1)
    {
        perror("reading /proc/pid/exe ");
        return 1;
    }
    line[len] = '\0';
    printf("Executable Path -- %s\n", line);
    return 0;
}