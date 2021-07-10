#include "nightswatch.h"

int end_key(int num)
{
    time_t beg = time(NULL);
    struct termios tty, newtty;
    if (tcgetattr(fileno(stdin), &tty) == -1)
    {
        perror("get terminal attributes ");
        return -1;
    }
    newtty = tty;
    newtty.c_lflag = newtty.c_lflag & ~(ICANON | ECHO);
    newtty.c_cc[VMIN] = 0;
    newtty.c_cc[VTIME] = 0;
    while (time(NULL) - beg < num)
    {
        if (tcsetattr(fileno(stdin), TCSANOW, &newtty) == -1)
        {
            perror("set terminal atributes ");
            return -1;
        }
        unsigned char ch;
        if (read(fileno(stdin), &ch, sizeof(ch)) == -1)
        {
            perror("read error ");
            return -1;
        }
        if (tcsetattr(fileno(stdin), TCSANOW, &tty) == -1)
        {
            perror("set terminal atributes ");
            return -1;
        }
        if (ch == 'q')
            return 0;
    }
    if (tcsetattr(fileno(stdin), TCSAFLUSH, &tty) == -1)
    {
        perror("set terminal atributes ");
        return -1;
    }
    return 1;
}

int newborn(int num)
{
    float useless;
    int unused;
    pid_t pid;
    do
    {
        FILE *o_file = fopen("/proc/loadavg", "r");
        if (o_file == NULL)
        {
            perror("proc/loadavg opening error ");
            return 1;
        }
        fseek(o_file, 0, SEEK_SET);
        fscanf(o_file, "%f %f %f %d/%d %d", &useless, &useless, &useless, &unused, &unused, &pid);
        printf("%d\n", pid);
        if (fclose(o_file) != 0)
        {
            perror("closing /proc/loadavg ");
            return 1;
        }
    } while (end_key(num));

    return 0;
}

int interrupt(int num)
{
    FILE *o_file = fopen("/proc/interrupts", "r");
    if (o_file == NULL)
    {
        perror("proc/interrupts opening error ");
        return 1;
    }
    unsigned long int size = 1000;
    char *line = (char *)malloc(size);

    getline(&line, &size, o_file);
    int beg = 1000, end = 0;
    for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++)
    {
        if (line[i] == 'C' && i < beg)
            beg = i;
        if (isdigit(line[i]) && i + 1 > end)
            end = i + 1;
    }
    line[end] = '\n';
    line[end + 1] = '\0';
    printf("%s\n", line + beg);
    if (fclose(o_file) != 0)
    {
        perror("closing /proc/interrupts ");
        return 1;
    }
    do
    {
        o_file = fopen("/proc/interrupts", "r");
        if (o_file == NULL)
        {
            perror("proc/interrupts opening error ");
            return 1;
        }
        getline(&line, &size, o_file);
        getline(&line, &size, o_file);
        getline(&line, &size, o_file);
        line[end] = '\n';
        line[end + 1] = '\0';
        printf("%s\n", line + beg);
        if (fclose(o_file) != 0)
        {
            perror("closing /proc/interrupts ");
            return 1;
        }
    } while (end_key(num));

    free(line);

    return 0;
}

int nightswatch(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    if (arg_num != 4)
    {
        printf("Incorrect number of arguments\n");
        return 1;
    }
    if (strcmp(arguments[1], "-n") != 0 || (strcmp(arguments[3], "newborn") != 0 && strcmp(arguments[3], "interrupt") != 0))
    {
        printf("Incorrect arguments\n");
        return 1;
    }
    int num = 0, len = strlen(arguments[2]);
    for (int i = 0; i < len; i++)
    {
        if (!isdigit(arguments[2][i]))
        {
            printf("Incorrect arguments\n");
            return 1;
        }
        num += (int)(arguments[2][i] - '0') * pow(10, len - 1 - i);
    }
    if (strcmp(arguments[3], "newborn") == 0)
        return newborn(num);
    else
        return interrupt(num);
}