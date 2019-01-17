#ifndef __connmgr_h__
#define __connmgr_h__

#include <stdio.h>
#include <unistd.h>
#include <mysql.h>
#include <inttypes.h>
#include <pthread.h>
#include "../include/config.h"
#include "../include/sensor_db.h"
#include "../include/shared_queue.h"

extern queue_ptr_t queue;

extern void write_to_fifo(char*);

void connmgr_run();

#endif
