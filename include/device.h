#ifndef DEVICE_H
#define DEVICE_H


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>


int init_dirver(void);


#endif