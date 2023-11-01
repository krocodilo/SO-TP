#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED


#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int randomNum(int min, int max);

int sendSignal(pid_t pid, int signum);

void delete_char(char *s,char ch);

int parseNum(char *str, int min, int max);

int create_and_open(char* pipename, int flags);

#endif