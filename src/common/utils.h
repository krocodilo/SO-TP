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
int readNextMessageType(int pipe_fd);
bool readNextMessage(int pipe_fd, void* buffer, int size);
bool writeMessage(int pipe_fd, int msgType, void* message, int size);



#endif //SO_TP_UTILS_H
