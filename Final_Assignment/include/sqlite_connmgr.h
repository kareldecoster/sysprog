#ifndef __SQLITE_CONNMGR_H__
#define __SQLITE_CONNMGR_H__
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "../include/config.h"
#include "../include/shared_queue.h"

//extern variables
int active;
extern queue_ptr_t queue;
//extern methods
extern void write_to_fifo(char* msg);

int sqlite_connmgr_run(void);

#endif
