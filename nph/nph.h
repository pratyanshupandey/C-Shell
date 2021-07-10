#ifndef __NPH_H
#define __NPH_H

#include "../headers.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int nph(char *arguments[]);
int jobs();
int kjob(char *arguments[]);
int fg(char *arguments[]);
int bg(char *arguments[]);
int overkill();

#endif