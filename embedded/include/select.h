#ifndef SELECT_H
#define SELECT_H

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <device.h>
#include <player.h>
#include <socket.h>


void init_select(void);
void menu();
void select_read_stdio();
void select_read_button();
void select_read_socket();
void select_read_serial();
void m_select();

#endif