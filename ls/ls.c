#include "ls.h"

int mx_link, mx_usr, mx_grp, mx_sz;
char *month[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"};

int max(int a, int b)
{
    if (a >= b)
        return a;
    return b;
}

// -    Regular file
// b    Block special file
// c    Character special file
// d    Directory
// l    Symbolic link
// n    Network file
// p    FIFO
// s    Socket

void printline(struct dirent *file, struct stat f_stat)
{
    char type;
    if (S_ISREG(f_stat.st_mode))
        type = '-';
    else if (S_ISDIR(f_stat.st_mode))
        type = 'd';
    else if (S_ISCHR(f_stat.st_mode))
        type = 'c';
    else if (S_ISBLK(f_stat.st_mode))
        type = 'b';
    else if (S_ISFIFO(f_stat.st_mode))
        type = 'p';
    else if (S_ISLNK(f_stat.st_mode))
        type = 'l';
    else if (S_ISSOCK(f_stat.st_mode))
        type = 's';

    printf("%c", type);
    printf("%c", f_stat.st_mode & S_IRUSR ? 'r' : '-');
    printf("%c", f_stat.st_mode & S_IWUSR ? 'w' : '-');
    printf("%c", f_stat.st_mode & S_IXUSR ? 'x' : '-');
    printf("%c", f_stat.st_mode & S_IRGRP ? 'r' : '-');
    printf("%c", f_stat.st_mode & S_IWGRP ? 'w' : '-');
    printf("%c", f_stat.st_mode & S_IXGRP ? 'x' : '-');
    printf("%c", f_stat.st_mode & S_IROTH ? 'r' : '-');
    printf("%c", f_stat.st_mode & S_IWOTH ? 'w' : '-');
    printf("%c ", f_stat.st_mode & S_IXOTH ? 'x' : '-');

    char format[100];
    sprintf(format, "%%%dld %%%ds %%%ds %%%dld ", mx_link, mx_usr, mx_grp, mx_sz);
    printf(format, f_stat.st_nlink, getpwuid(f_stat.st_uid)->pw_name, getgrgid(f_stat.st_gid)->gr_name, f_stat.st_size);
    struct tm *mod_tm = gmtime(&(f_stat.st_mtim.tv_sec));
    printf("%3s %2d ", month[mod_tm->tm_mon], mod_tm->tm_mday);
    time_t diff = time(NULL) - f_stat.st_mtim.tv_sec;
    if (diff > 15552000)
        printf("%5d", mod_tm->tm_year + 1900);
    else
        printf("%02d:%02d", mod_tm->tm_hour, mod_tm->tm_min);
    printf(" %s\n", file->d_name);
    return;
}

int print_ls(char dir_path[], int l_flag, int a_flag)
{
    DIR *directory = opendir(dir_path);
    if (directory == NULL)
    {
        perror("Directory ");
        return 1;
    }
    struct stat f_stat;
    mx_link = mx_usr = mx_grp = mx_sz = 0;
    long long int total_blk = 0;
    while (3)
    {
        struct dirent *file = readdir(directory);
        if (file == NULL)
            break;
        if (l_flag == 0)
        {
            if (file->d_name[0] != '.' || (file->d_name[0] == '.' && a_flag))
                printf("%s\n", file->d_name);
        }
        else
        {
            char file_path[5000] = {0};
            strcpy(file_path, dir_path);
            if (file_path[strlen(file_path) - 1] != '/')
                strcat(file_path, "/");
            strcat(file_path, file->d_name);
            if (stat(file_path, &f_stat) == -1)
            {
                perror(file->d_name);
                return 1;
            }
            if (file->d_name[0] != '.' || (file->d_name[0] == '.' && a_flag))
            {
                total_blk += f_stat.st_blocks;
                mx_link = max(mx_link, 1 + (int)log10(f_stat.st_nlink));
                mx_usr = max(mx_usr, strlen(getpwuid(f_stat.st_uid)->pw_name));
                mx_grp = max(mx_grp, strlen(getgrgid(f_stat.st_gid)->gr_name));
                mx_sz = max(mx_sz, 1 + (int)log10(f_stat.st_size));
            }
        }
    }
    closedir(directory);
    if (l_flag == 0)
        return 0;
    directory = opendir(dir_path);
    if (directory == NULL)
    {
        perror("Directory ");
        return 1;
    }
    printf("total %lld\n", total_blk / 2);
    while (3)
    {
        struct dirent *file = readdir(directory);
        if (file == NULL)
            break;
        char file_path[5000] = {0};
        strcpy(file_path, dir_path);
        if (file_path[strlen(file_path) - 1] != '/')
            strcat(file_path, "/");
        strcat(file_path, file->d_name);
        if (stat(file_path, &f_stat) == -1)
        {
            perror("file stat error ");
            return 1;
        }
        if (file->d_name[0] != '.' || (file->d_name[0] == '.' && a_flag))
            printline(file, f_stat);
    }
    closedir(directory);
    return 0;
}

int ls(char *arguments[], char initial_dir[], int ini_dir_length)
{
    int a_flag = 0, l_flag = 0, dir_count = 0, ret = 0;
    int i = 1;
    while (arguments[i] != NULL)
    {
        if (arguments[i][0] != '-')
        {
            dir_count++;
            i++;
            continue;
        }
        int len = strlen(arguments[i]);
        for (int j = 0; j < len; j++)
        {
            if (arguments[i][j] == 'a')
                a_flag = 1;
            else if (arguments[i][j] == 'l')
                l_flag = 1;
        }
        i++;
    }
    if (dir_count == 0)
    {
        char curr_dir[4500];
        if (getcwd(curr_dir, 4500) == NULL)
        {
            perror("PWD Error ");
            return 1;
        }
        ret |= print_ls(curr_dir, l_flag, a_flag);
    }
    else
    {
        i = 1;
        while (arguments[i] != NULL)
        {
            if (arguments[i][0] != '-')
            {
                dir_count--;
                if (arguments[i][0] == '~')
                {
                    strcat(initial_dir, arguments[i] + 1);
                    arguments[i] = initial_dir;
                }
                printf("%s:\n", arguments[i]);
                ret |= print_ls(arguments[i], l_flag, a_flag);
                if (dir_count)
                    printf("\n");
                initial_dir[ini_dir_length] = '\0';
            }
            i++;
        }
    }
    return ret;
}