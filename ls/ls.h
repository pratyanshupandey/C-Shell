#ifndef __LS_H
#define __LS_H

#include "../headers.h"
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>

int ls(char *arguments[], char initial_dir[], int ini_dir_length);
int print_ls(char dir_path[], int l_flag, int a_flag);
int max(int a, int b);
void printline(struct dirent *file, struct stat f_stat);

#endif