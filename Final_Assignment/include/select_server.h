#ifndef __select_server_h__
#define __select_server_h__
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../include/config.h"
#include "../include/mylist.h"
#include "../include/shared_queue.h"
#include "../include/tcpsocket.h"

extern queue_ptr_t queue;
extern void write_to_fifo(char*);

int server_run(void);

#endif
