#include "nph.h"

volatile sig_atomic_t is_sigstp = 0;
volatile sig_atomic_t is_sigint = 0;

extern char *back_ground[40];
extern int bp_id[40];
extern int bp_num[40];
extern int bp_count;
extern int bpnum;

void handler(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) != -1)
    {
        if (bp_count != 0 && pid == -1)
        {
            perror("waitpid ");
            return;
        }
        int i;
        for (i = 0; i < 40; i++)
        {
            if (bp_id[i] == pid)
                break;
        }
        if (pid > 0)
        {
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) == 0)
                    printf("\n%s with pid %d exited normally\n", back_ground[i], bp_id[i]);
                else
                    printf("\n%s with pid %d exited abnormally with exit code %d\n", back_ground[i], bp_id[i], WEXITSTATUS(status));
                for (int j = i; j < 39; j++)
                {
                    bp_id[j] = bp_id[j + 1];
                    back_ground[j] = back_ground[j + 1];
                    bp_num[j] = bp_num[j + 1];
                }
                free(back_ground[bp_count]);
                bp_count--;
            }
        }
    }
}

void child_stop(int sig)
{
    if (sig == SIGINT && raise(SIGKILL) == -1)
    {
        perror("kill ");
        exit(1);
    }
    else
    {
        int fg_pgid = tcgetpgrp(STDERR_FILENO);
        if (getpgrp() == fg_pgid)
        {
            if (sig == SIGTSTP && setpgid(0, 0) == -1)
            {
                perror("setting processs group ");
                exit(1);
            }
        }
        if (raise(SIGSTOP) != 0)
        {
            perror("raise ");
            exit(1);
        }
    }
}

void parent_stop(int sig)
{
    if (sig == SIGINT)
        is_sigint = 1;
    if (sig == SIGTSTP)
        is_sigstp = 1;
}

int nph(char *arguments[])
{
    int is_bg = 0, i = 0;
    while (arguments[i] != NULL)
    {
        if (strcmp(arguments[i], "&") == 0)
        {
            is_bg = 1;
            arguments[i] = NULL;
        }
        i++;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork error ");
        return 1;
    }
    else if (pid == 0)
    {
        struct sigaction stphandler;
        memset(&stphandler, 0, sizeof(stphandler));
        stphandler.sa_handler = child_stop;
        if (sigaction(SIGTSTP, &stphandler, NULL) == -1)
        {
            perror("sigaction ");
            exit(1);
        }
        if (sigaction(SIGINT, &stphandler, NULL) == -1)
        {
            perror("sigaction ");
            exit(1);
        }
        if (is_bg && setpgid(0, 0) == -1)
        {
            perror("setting processs group ");
            exit(1);
        }
        if (execvp(arguments[0], arguments) == -1)
        {
            perror(arguments[0]);
            exit(1);
        }
    }
    else
    {
        tcsetpgrp(STDERR_FILENO, getpgrp());
        if (is_bg)
        {
            setpgid(pid, pid);
            signal(SIGCHLD, handler);
            bp_id[bp_count] = pid;
            bp_num[bp_count] = bpnum;
            back_ground[bp_count] = (char *)malloc(100);
            strcpy(back_ground[bp_count], arguments[0]);
            bp_count++;
            bpnum++;
            printf("%s is being executed in background with pid %d\n", arguments[0], pid);
        }
        else
        {
            struct sigaction new_handler, old_handler;
            memset(&new_handler, 0, sizeof(new_handler));
            new_handler.sa_handler = parent_stop;
            if (sigaction(SIGTSTP, &new_handler, &old_handler) == -1)
            {
                perror("sigaction ");
                return 1;
            }

            int status;
            pid_t ch_pid;
            do
            {
                ch_pid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status) && ch_pid != pid);

            int ret = 0;
            if (is_sigstp == 1)
            {
                is_sigstp = 0;
                bp_id[bp_count] = pid;
                bp_num[bp_count] = bpnum;
                back_ground[bp_count] = (char *)malloc(100);
                strcpy(back_ground[bp_count], arguments[0]);
                bp_count++;
                bpnum++;
                ret = 1;
            }
            tcsetpgrp(STDERR_FILENO, getpgrp());
            if (sigaction(SIGTSTP, &old_handler, NULL) == -1)
            {
                perror("sigaction ");
                return 1;
            }
            if (ret)
                return ret;
            return WEXITSTATUS(status);
        }
        tcsetpgrp(STDERR_FILENO, getpgrp());
    }
    return 0;
}

int jobs()
{
    char path[30];
    pid_t pid;
    for (int i = 0; i < bp_count; i++)
    {
        pid = bp_id[i];
        if (kill(pid, 0) == -1)
        {
            for (int j = i; j < 39; j++)
            {
                bp_id[j] = bp_id[j + 1];
                back_ground[j] = back_ground[j + 1];
                bp_num[j] = bp_num[j + 1];
            }
            free(back_ground[bp_count]);
            bp_count--;
            i--;
            continue;
        }
        sprintf(path, "/proc/%d/stat", pid);
        FILE *o_file = fopen(path, "r");
        if (o_file == NULL)
        {
            perror("proc opening error ");
            return 1;
        }
        char left[30], status;
        fscanf(o_file, "%d %s %c", &pid, left, &status);
        if (fclose(o_file) != 0)
        {
            perror("closing /proc/pid/stat ");
            return 1;
        }
        printf("[%d] ", bp_num[i]);
        switch (status)
        {
        case 'R':
            printf("Running");
            break;

        case 'S':
            printf("Sleeping");
            break;

        case 'D':
            printf("Waiting in uninterruptible disk sleep");
            break;

        case 'Z':
            printf("Zombie");
            break;

        case 'T':
            printf("Stopped");
            break;

        case 'X':
            printf("Dead");
            break;

        case 't':
            printf("Tracing stop");
            break;

        default:
            break;
        }
        printf(" %s [%d]\n", back_ground[i], bp_id[i]);
    }
}

int kjob(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    if (arg_num < 3)
    {
        printf("shell: too less arguments");
        return 1;
    }
    else if (arg_num > 3)
    {
        printf("shell: too many arguments");
        return 1;
    }

    int num = 0;
    int len = strlen(arguments[1]);
    for (int i = 0; i < len; i++)
        num += (int)(arguments[1][i] - '0') * pow(10, len - 1 - i);

    int sig = 0;
    len = strlen(arguments[2]);
    for (int i = 0; i < len; i++)
        sig += (int)(arguments[2][i] - '0') * pow(10, len - 1 - i);

    for (int i = 0; i < bp_count; i++)
    {
        if (bp_num[i] == num)
        {
            if (kill(bp_id[i], sig) == -1)
            {
                perror("kill ");
                return 1;
            }
            return 0;
        }
    }
    printf("shell: No job with the given job number exists\n");
    return 1;
}

int fg(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    if (arg_num < 2)
    {
        printf("shell: too less arguments");
        return 1;
    }
    else if (arg_num > 2)
    {
        printf("shell: too many arguments");
        return 1;
    }

    int num = 0;
    int len = strlen(arguments[1]);
    for (int i = 0; i < len; i++)
        num += (int)(arguments[1][i] - '0') * pow(10, len - 1 - i);

    pid_t pid = 0;
    int i;
    for (i = 0; i < bp_count; i++)
    {
        if (bp_num[i] == num)
        {
            pid = bp_id[i];
            break;
        }
    }
    if (pid == 0)
    {
        printf("shell: No job with the given job number exists\n");
        return 1;
    }

    setpgid(pid, getpgrp());
    kill(pid, SIGCONT);

    tcsetpgrp(STDERR_FILENO, getpgrp());
    struct sigaction new_handler, old_handler;
    memset(&new_handler, 0, sizeof(new_handler));
    new_handler.sa_handler = parent_stop;
    if (sigaction(SIGTSTP, &new_handler, &old_handler) == -1)
    {
        perror("sigaction ");
        return 1;
    }
    if (sigaction(SIGINT, &new_handler, NULL) == -1)
    {
        perror("sigaction ");
        return 1;
    }
    tcsetpgrp(STDERR_FILENO, getpgrp());

    int status;
    pid_t ch_pid;
    do
    {
        ch_pid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status) && ch_pid != pid);

    if (is_sigstp == 0 || is_sigint == 1)
    {
        for (int j = i; j < 39; j++)
        {
            bp_id[j] = bp_id[j + 1];
            back_ground[j] = back_ground[j + 1];
            bp_num[j] = bp_num[j + 1];
        }
        free(back_ground[bp_count]);
        bp_count--;
    }
    if (is_sigint == 1)
    {
        is_sigint = 0;
        if (kill(pid, SIGKILL) == -1)
        {
            perror("kill ");
            return 1;
        }
    }
    if (is_sigstp == 1)
    {
        is_sigstp = 0;
        if (kill(pid, SIGSTOP) != 0)
        {
            perror("kill ");
            exit(1);
        }
    }
    tcsetpgrp(STDERR_FILENO, getpgrp());

    if (sigaction(SIGINT, &old_handler, NULL) == -1)
    {
        perror("sigaction ");
        return 1;
    }
    if (sigaction(SIGTSTP, &old_handler, NULL) == -1)
    {
        perror("sigaction ");
        return 1;
    }
    tcsetpgrp(STDERR_FILENO, getpgrp());

    return 0;
}

int bg(char *arguments[])
{
    int arg_num = no_of_arg(arguments);
    if (arg_num < 2)
    {
        printf("shell: too less arguments");
        return 1;
    }
    else if (arg_num > 2)
    {
        printf("shell: too many arguments");
        return 1;
    }

    int num = 0;
    int len = strlen(arguments[1]);
    for (int i = 0; i < len; i++)
        num += (int)(arguments[1][i] - '0') * pow(10, len - 1 - i);

    for (int i = 0; i < bp_count; i++)
    {
        if (bp_num[i] == num)
        {
            kill(bp_id[i], SIGCONT);
            return 0;
        }
    }
    printf("shell: No job with the given job number exists\n");
    return 1;
}

int overkill()
{
    while (bp_count)
    {
        if (kill(bp_id[0], SIGKILL) == -1)
        {
            perror("kill ");
            return 1;
        }
        for (int j = 0; j < 39; j++)
        {
            bp_id[j] = bp_id[j + 1];
            back_ground[j] = back_ground[j + 1];
            bp_num[j] = bp_num[j + 1];
        }
        free(back_ground[bp_count]);
        bp_count--;
    }
    return 0;
}