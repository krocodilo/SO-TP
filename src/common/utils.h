#ifndef SO_TP_UTILS_H
#define SO_TP_UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


int create_and_open(char* pipename, int flags);


#endif //SO_TP_UTILS_H
