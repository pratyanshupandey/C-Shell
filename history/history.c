#include "history.h"

int read_history(char *arguments[], char initial_dir[], int ini_dir_length)
{
    int num = 0;
    if (arguments[1] == NULL)
        num = 10;
    else
    {
        int len = strlen(arguments[1]);
        for (int i = 0; i < len; i++)
            num += (int)(arguments[1][i] - '0') * pow(10, len - 1 - i);
    }

    strcat(initial_dir, "/.history");
    FILE *o_file = fopen(initial_dir, "r");
    initial_dir[ini_dir_length] = '\0';
    if (o_file == NULL)
    {
        perror("history opening error ");
        return 1;
    }

    unsigned long int size = 5000, count = 0;
    char *line = (char *)malloc(size);
    while (getline(&line, &size, o_file) > 0)
        count++;
    if (num > count)
        num = count;

    count -= num;
    int l = 0;
    fseek(o_file, 0, SEEK_SET);

    while (getline(&line, &size, o_file) > 0)
    {
        if (l >= count)
            printf("%s", line);
        l++;
    }

    if (fclose(o_file) != 0)
    {
        perror("closing history file ");
        return 1;
    }
    free(line);
}

int write_history(char command[], char initial_dir[], int ini_dir_length)
{
    int flag = 0, j = 0;
    while (command[j] != '\0')
    {
        if (command[j] != ' ' && command[j] != '\t' && command[j] != '\n' && command[j] != '\r' && command[j] != '\v' && command[j] != '\f')
        {
            flag = 1;
            break;
        }
        j++;
    }
    if (flag == 0)
        return 0;

    char temp_path[5000];
    strcpy(temp_path, initial_dir);
    strcat(initial_dir, "/.history");
    FILE *o_file = fopen(initial_dir, "r");
    if (o_file == NULL)
    {
        perror("history opening error ");
        return 1;
    }
    strcat(temp_path, "/.temphistory");
    FILE *a_file = fopen(temp_path, "w+");
    if (a_file == NULL)
    {
        perror("file creation error ");
        return 1;
    }

    unsigned long int size = 5000, count = 0;
    char *line = (char *)malloc(size);
    while (getline(&line, &size, o_file) > 0)
        count++;

    fseek(o_file, 0, SEEK_SET);

    if (count == 20)
        getline(&line, &size, o_file);

    while (getline(&line, &size, o_file) > 0)
        fwrite(line, 1, strlen(line), a_file);
    fwrite(command, 1, strlen(command), a_file);
    if (fclose(o_file) != 0)
    {
        perror("closing history ");
        return 1;
    }
    if (fclose(a_file) != 0)
    {
        perror("closing history ");
        return 1;
    }
    remove(initial_dir);
    rename(temp_path, initial_dir);
    free(line);
    initial_dir[ini_dir_length] = '\0';
    return 0;
}