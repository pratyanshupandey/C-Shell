#ifndef __NIGHTSWATCH_H
#define __NIGHTSWATCH_H

#include "../headers.h"
#include <time.h>
#include <termios.h>

int nightswatch(char *arguments[]);
int newborn(int num);
int interrupt(int num);
int end_key(int num);

#endif