#ifndef SOCKET_H
#define SOCKET_H


#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>



#define PORT        8000
#define IP          "127.0.0.1"


int init_socket(void);

#endif // !1