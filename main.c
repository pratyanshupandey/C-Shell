#include "headers.h"
#include "prompt/prompt.h"
#include "builtins/cd.h"
#include "builtins/pwd.h"
#include "builtins/echo.h"
#include "builtins/env.h"
#include "ls/ls.h"
#include "nph/nph.h"
#include "pinfo/pinfo.h"
#include "history/history.h"
#include "nightswatch/nightswatch.h"

extern int errno;

char initial_dir[4500];
char prev_dir[4500];
int ini_dir_length;

char *back_ground[40] = {0};
int bp_id[40] = {0};
int bp_num[40] = {0};
int bp_count = 0;
int bpnum = 1;

int redirect(char *arguments[], int fin, int fout)
{
    int orig_fin, orig_fout, ret = 0;
    if (fin)
    {
        orig_fin = dup(0);
        if (dup2(fin, 0) != 0)
        {
            perror("redir fin");
            return 1;
        }
        if (close(fin) == -1)
        {
            perror("closing fin");
            return 1;
        }
    }
    if (fout != 1)
    {
        orig_fout = dup(1);
        if (dup2(fout, 1) != 1)
        {
            perror("redir fout");
            return 1;
        }
        if (close(fout) == -1)
        {
            perror("closing fout");
            return 1;
        }
    }

    if (strcmp(arguments[0], "cd") == 0)
        ret = cd(arguments, initial_dir, ini_dir_length);
    else if (strcmp(arguments[0], "pwd") == 0)
        ret = pwd(arguments);
    else if (strcmp(arguments[0], "echo") == 0)
        ret = echo(arguments);
    else if (strcmp(arguments[0], "ls") == 0)
        ret = ls(arguments, initial_dir, ini_dir_length);
    else if (strcmp(arguments[0], "pinfo") == 0)
        ret = pinfo(arguments);
    else if (strcmp(arguments[0], "history") == 0)
        ret = read_history(arguments, initial_dir, ini_dir_length);
    else if (strcmp(arguments[0], "nightswatch") == 0)
        ret = nightswatch(arguments);
    else if (strcmp(arguments[0], "setenv") == 0)
        ret = setval(arguments);
    else if (strcmp(arguments[0], "unsetenv") == 0)
        ret = unsetval(arguments);
    else if (strcmp(arguments[0], "jobs") == 0)
        ret = jobs();
    else if (strcmp(arguments[0], "kjob") == 0)
        ret = kjob(arguments);
    else if (strcmp(arguments[0], "fg") == 0)
        ret = fg(arguments);
    else if (strcmp(arguments[0], "bg") == 0)
        ret = bg(arguments);
    else if (strcmp(arguments[0], "overkill") == 0)
        ret = overkill();
    else if (strcmp(arguments[0], "quit") == 0)
        exit(0);
    else
        ret = nph(arguments);

    if (fout != 1)
    {
        if (dup2(orig_fout, 1) != 1)
        {
            perror("redir orig_fout");
            return 1;
        }
        if (close(orig_fout) == -1)
        {
            perror("closing orig_fout");
            return 1;
        }
    }
    if (fin)
    {
        if (dup2(orig_fin, 0) != 0)
        {
            perror("redir orig_fin");
            return 1;
        }
        if (close(orig_fin) == -1)
        {
            perror("closing orig_fin");
            return 1;
        }
    }
    return ret;
}

int piped(char *arguments[])
{
    int i = 0, com_pos[10] = {0}, j = 1, ret = 0;
    while (arguments[i] != NULL)
    {
        if (strcmp(arguments[i], "|") == 0)
        {
            arguments[i] = NULL;
            com_pos[j] = i + 1;
            j++;
        }
        i++;
    }
    int fin = in_fd(arguments), fout;
    if (fin == -1)
        return 1;
    for (i = 0; i < j - 1; i++)
    {
        int fdpipe[2];
        if (pipe(fdpipe) == -1)
        {
            perror("pipe");
            return 1;
        }
        fout = fdpipe[1];
        redirect(arguments + com_pos[i], fin, fout);
        if (fin != 0)
            close(fin);
        if (fout != 1)
            close(fout);
        fin = fdpipe[0];
    }
    fout = out_fd(arguments + com_pos[i]);
    if (fout == -1)
        return 1;
    ret = redirect(arguments + com_pos[i], fin, fout);
    if (fin != 0)
        close(fin);
    if (fout != 1)
        close(fout);
    return ret;
}

int execute(char command[])
{
    char *arguments[100];
    int arg_num, is_pipe = 0;
    for (arg_num = 0; arg_num < 100; arg_num++)
    {
        if (arg_num == 0)
            arguments[arg_num] = strtok(command, " ");
        else
            arguments[arg_num] = strtok(NULL, " ");
        if (arguments[arg_num] == NULL)
            break;
    }
    if (arguments[0] == NULL)
        return 0;
    int aor[30][2], i = 0, j = 0;
    while (arguments[i] != NULL)
    {
        if (arguments[i][0] == '@')
        {
            arguments[i] = NULL;
            aor[j][0] = i;
            aor[j][1] = 0;
            j++;
        }
        else if (arguments[i][0] == '$')
        {
            arguments[i] = NULL;
            aor[j][0] = i;
            aor[j][1] = 1;
            j++;
        }
        i++;
    }
    int ret = piped(arguments);
    for (i = 0; i < j; i++)
    {
        if ((ret == 1 && aor[i][1] == 0) || (ret == 0 && aor[i][1] == 1))
            continue;
        ret = piped(arguments + aor[i][0] + 1);
    }
    return ret;
}

int main(int argc, char const *argv[])
{
    unsigned long size = 5000;
    int command_len;
    char *command = (char *)malloc(size);

    if (getcwd(initial_dir, 4500) == NULL)
    {
        perror("Initial Directory Error ");
        return 1;
    }
    ini_dir_length = strlen(initial_dir);
    strcpy(prev_dir, initial_dir);

    FILE *h_file = fopen(".history", "a+");
    if (h_file == NULL)
        perror("closing history ");
    if (fclose(h_file) != 0)
        perror("closing history ");

    struct sigaction parent_stop;
    memset(&parent_stop, 0, sizeof(parent_stop));
    parent_stop.sa_handler = SIG_IGN;
    if (sigaction(SIGTSTP, &parent_stop, NULL) == -1)
        perror("sigaction ");
    if (sigaction(SIGINT, &parent_stop, NULL) == -1)
        perror("sigaction ");

    int ex_code = 0;
    while (1)
    {
        if (prompt(initial_dir, ini_dir_length, ex_code))
            return 1;

        raise(SIGCONT);
        tcsetpgrp(STDIN_FILENO, getpgrp());
        int bytes_read = getline(&command, &size, stdin);

        if (bytes_read == -1)
        {
            if (errno == 0)
            {
                printf("\n");
                exit(0);
            }
        }
        write_history(command, initial_dir, ini_dir_length);

        command_len = bytes_read - 1;
        if (command_len <= 0)
            continue;
        command[command_len] = '\0';
        int com_beg[20] = {0}, j = 1;
        for (int i = 0; i < command_len; i++)
            if (command[i] == '\t' || command[i] == '\n' || command[i] == '\r' || command[i] == '\v' || command[i] == '\f')
                command[i] = ' ';
            else if (command[i] == ';')
            {
                command[i] = '\0';
                if (command[i + 1] != '\0')
                    com_beg[j++] = i + 1;
            }
        ex_code = 0;
        for (int i = 0; i < j; i++)
            ex_code = execute(command + com_beg[i]);
    }
    return 0;
}