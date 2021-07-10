#ifndef __HEADERS_H
#define __HEADERS_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int input(char *arguments[], char **inp);
int output(char *arguments[], char **out);
int redir_inp(char *arguments[]);
int redir_out(char *arguments[]);
int out_fd(char *arguments[]);
int in_fd(char *arguments[]);
int no_of_arg(char *arguments[]);

#endif