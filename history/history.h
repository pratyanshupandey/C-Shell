#ifndef __HISTORY_H
#define __HISTORY_H

#include "../headers.h"

int read_history(char *arguments[], char initial_dir[], int ini_dir_length);
int write_history(char command[], char initial_dir[], int ini_dir_length);

#endif