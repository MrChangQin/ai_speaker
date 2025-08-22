#ifndef DEVICE_H
#define DEVICE_H


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>


int init_dirver(void);
void start_buzzer();
int get_key_id();
int init_serial();

#endif