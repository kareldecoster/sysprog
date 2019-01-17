#ifndef __DATAMGR_H__
#define __DATAMGR_H__

#include "../include/config.h"
#include "../include/mylist.h"
#include "../include/shared_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


extern void write_to_fifo(char*);
extern int active;
extern queue_ptr_t queue;

int mylist_errno;
double min_value;
double max_value;

struct sensor;
typedef struct sensor sensor_t;

/*
* Make and initialize the sensor mylist.
*/
mylist_ptr_t datamgr_create_sensor_mylist();

/*
* Free the mylist
*/
void datamgr_free_sensor_mylist(mylist_ptr_t* mylist);

/*
*Returns a pointer to the sensor from the mylist 'mylist' with sensor-id 'id'.
*Returns NULL if the mylist points to NULL or when there was no sensor with id 'id' in the mylist.
*/
sensor_t* datamgr_get_sensor_by_id(mylist_ptr_t mylist, int id);

/*
*Adds a value to the sensor and recalculates the average.
*/
void datamgr_add_value_to_sensor(double value,time_t timestamp, sensor_t* sensor);


/*
*	Returns the average for the sensor 'sensor'.
*/
double datamgr_get_average_of_sensor(sensor_t* sensor);

/*
*	Run method for the datamgr	
*/
void datamgr_run();

#endif
