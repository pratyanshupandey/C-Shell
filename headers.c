#include "headers.h"

void shift(char *arguments[], int i)
{
    while (arguments[i] != NULL)
    {
        arguments[i] = arguments[i + 2];
        i++;
    }
}

int input(char *arguments[], char **inp)
{
    int i = 0;
    while (arguments[i] != NULL)
    {
        if (strcmp(arguments[i], "<") == 0)
        {
            if (arguments[i + 1] == NULL)
                return -1;
            *inp = arguments[i + 1];
            shift(arguments, i);
            return 1;
        }
        i++;
    }
    return 0;
}
int output(char *arguments[], char **out)
{
    int i = 0;
    while (arguments[i] != NULL)
    {
        if (strcmp(arguments[i], ">") == 0)
        {
            if (arguments[i + 1] == NULL)
                return -1;
            *out = arguments[i + 1];
            shift(arguments, i);
            return 1;
        }
        else if (strcmp(arguments[i], ">>") == 0)
        {
            if (arguments[i + 1] == NULL)
                return -1;
            *out = arguments[i + 1];
            shift(arguments, i);
            return 2;
        }
        i++;
    }
    return 0;
}

int redir_inp(char *arguments[])
{
    char *inp;
    if (input(arguments, &inp) > 0)
    {
        int fin = open(inp, O_RDONLY);
        if (fin == -1)
        {
            perror(inp);
            return 0;
        }
        if (dup2(fin, 0) != 0)
        {
            perror("redir");
            return 1;
        }
        if (close(fin) == -1)
        {
            perror("closing");
            return 1;
        }
    }
}
int redir_out(char *arguments[])
{
    char *out;
    int val = output(arguments, &out), fout;
    if (val > 0)
    {
        if (val == 1)
            fout = open(out, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        else
            fout = open(out, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fout == -1)
        {
            perror(out);
            return 0;
        }
        if (dup2(fout, 1) != 1)
        {
            perror("redir");
            return 1;
        }
        if (close(fout) == -1)
        {
            perror("closing");
            return 1;
        }
    }
}

int in_fd(char *arguments[])
{
    char *inp;
    int val = input(arguments, &inp);
    if (val == 1)
    {
        int fin = open(inp, O_RDONLY);
        if (fin == -1)
        {
            perror(inp);
            return -1;
        }
        return fin;
    }
    return val;
}

int out_fd(char *arguments[])
{
    char *out;
    int val = output(arguments, &out), fout;
    if (val > 0)
    {
        if (val == 1)
            fout = open(out, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        else
            fout = open(out, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fout == -1)
        {
            perror(out);
            return -1;
        }
        return fout;
    }
    else if (val == 0)
        return 1;
    else
        return -1;
}

int no_of_arg(char *arguments[])
{
    int i = 0;
    while (arguments[i] != NULL)
    {
        i++;
    }
    return i;
}