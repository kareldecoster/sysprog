#include "../include/datamgr.h"

#ifndef SET_MIN_TEMP
#error PLEASE DEFINE SET_MIN_TEMP.
#endif

#ifndef SET_MIN_TEMP
#error PLEASE DEFINE SET_MAX_TEMP.
#endif

#define SENSOR_BUFFER_SIZE 5

int mylist_errno;
double min_value = SET_MIN_TEMP;
double max_value = SET_MAX_TEMP;

typedef struct sensor sensor_t;


struct sensor {
	int id;
	int room;
	double average_value;
	double values[5];
	int index;
	bool buffered;
	time_t timestamp;
};

/*
 * Copy the 'content' of src_element to dst_element.
 */
void myelement_copy(element_ptr_t *dest_element, element_ptr_t src_element)
{
 	sensor_t* sens = malloc(sizeof(sensor_t));
 	if(sens!=NULL){
		sens->id = ((sensor_t*)src_element)->id;
		sens->room = ((sensor_t*)src_element)->room;
		sens->average_value = ((sensor_t*)src_element)->average_value;
		sens->values[0] = ((sensor_t*)src_element)->values[0];
		sens->values[1] = ((sensor_t*)src_element)->values[1];
		sens->values[2] = ((sensor_t*)src_element)->values[2];
		sens->values[3] = ((sensor_t*)src_element)->values[3];
		sens->values[4] = ((sensor_t*)src_element)->values[4];
		sens->index = ((sensor_t*)src_element)->index;
		sens->buffered = ((sensor_t*)src_element)->buffered;
		sens->timestamp = ((sensor_t*)src_element)->timestamp;
	
		*dest_element = (void*)sens;
 	}
}


/*
 * Clean up element, including freeing memory if needed
 */
void myelement_free(element_ptr_t *element)
{
  free(*element);
  
}

/*
 * Print 1 sensor element to stdout. 
 */
void myelement_print(element_ptr_t element)
{
	sensor_t* sens = (sensor_t*) element;
 	printf("Sensor id : %d, room-id : %d , average value: %lf\n", sens->id, sens->room, sens->average_value);
 	
}

/*
 * Compare two sensor element elements on id; returns -1, 0 or 1 
 */
int myelement_compare(element_ptr_t x, element_ptr_t y)
{
	sensor_t* a = (sensor_t*) x;
	sensor_t* b = (sensor_t*) y;
  if(a->id == b->id){
  
  	return 0;
  	
  }
  if(a->id > b->id){
  
  	return 1;
  	
  }else{
  
  	return -1;
  	
  }
}

/*
* Make and initialize the sensor mylist.
*/
mylist_ptr_t datamgr_create_sensor_mylist(){

	mylist_ptr_t mylist = NULL;
 	mylist = mylist_create( &myelement_copy, &myelement_free, &myelement_compare, &myelement_print);
 	if(mylist == NULL){
 		write_to_fifo("No mylist of nodes could be created. \n");
 		
 		return NULL;
 		
 	}
 	FILE *ifp;
 	ifp = fopen("../res/room_sensor.map", "r");
 	if(ifp == NULL){
 		char* buffer = malloc(70);
 		buffer = getcwd(buffer, 70);
 		char cbuf[70];
 		sprintf(cbuf,"Could not load room_sensor.map, make sure the file is in this directory: %s \n", buffer);
 		write_to_fifo(cbuf);
 		free(buffer);
 		buffer = NULL;
 		
 	}
 	//Make all the sensors in the mylist.
	sensor_t* sensor = malloc(sizeof(sensor_t));
	if(sensor == NULL){
		write_to_fifo("Error allocating memory for sensor.\n");
		fclose(ifp);
		
		return NULL;
		
	}
	char* s = malloc(20);
	char* prev;
 	while ( ( s = fgets( s,20, ifp ))!= NULL ){
 		char* c ;
 		c = strtok (s," ");
 		sensor->room = atoi(c);
 		c = strtok (NULL, "\n");
 		sensor->id = atoi(c);
 		sensor->index = 0;
 		sensor->buffered = false;	
 		mylist = mylist_insert_sorted(mylist, sensor);
 		
		prev = s;
 	}
 	fclose(ifp);
 	free(prev);
 	prev = NULL;
 	free(sensor);
 	sensor = NULL;
 	 
	return mylist;

}

/*
*Adds a value to the sensor and recalculates the average.
*/
void datamgr_add_value_to_sensor(double value,time_t timestamp, sensor_t* sensor){
	if(sensor != NULL){
		sensor->values[sensor->index] = value;
		if(sensor->index==4 && sensor->buffered!=true){
			sensor->buffered = true;
			
		}
		sensor->index = (sensor->index + 1)%SENSOR_BUFFER_SIZE;
		//If the sensor is buffered, recalculate the average.
		if(sensor->buffered == true){
			int i = 0;
			double sum = 0;
			for(i = 0; i < SENSOR_BUFFER_SIZE; i++){
				sum += sensor->values[i];
				
			}
			sensor->average_value = sum / SENSOR_BUFFER_SIZE;
			//If the average is outsi<de of the MIN and MAX size, create a log file.
			if(sensor->average_value > max_value){
				char str[70] ;
				sprintf(str,"Room %d is too hot. Temperature : %lf °C\n", sensor->room, sensor->average_value);
				write_to_fifo(str);
			
			}
			if(sensor-> average_value < min_value){
				char str[70] ;
				sprintf(str,"Room %d is too cold. Temperatuer : %lf °C\n", sensor->room, sensor->average_value);
				write_to_fifo(str);
			
			}
			
		}
		//Update the timestamp.
		sensor->timestamp = timestamp;
	}
}

/*
*Returns a sensor pointer to the sensor with the sensor-id id from mylist mylist.
*If no sensor with sensor-id id is found, NULL is returned.
*If the mylist that was handed to this function is NULL, NULL will be returned.
*/
sensor_t* datamgr_get_sensor_by_id(mylist_ptr_t mylist, int id){
	if(mylist!=NULL){
		int i = 0;
		for(i=0;i<mylist_size(mylist);i++){
			sensor_t* sense = (sensor_t*)mylist_get_element_at_index(mylist,i);
			if(sense->id == id){
			
				return sense;
				
			}
		}
		
		return NULL;
	
	}else{
	
		return NULL;
		
	}
}



/*
* Free the mylist
*/
void datamgr_free_sensor_mylist(mylist_ptr_t* mylist){
	mylist_free(mylist);

}

/*
*	Returns the average for the sensor 'sensor'.
*/
double datamgr_get_average_of_sensor(sensor_t* sensor){
	return sensor->average_value;

}

void datamgr_run(){
	mylist_ptr_t mylist = datamgr_create_sensor_mylist();
	sensor_data_ptr_t* prev=NULL;
	while(active){
		sensor_data_ptr_t* data = (sensor_data_ptr_t*)queue_top(queue);
		if( *data != NULL && data!=prev){
			sensor_data_ptr_t dummy = *data;
			prev = data;
			sensor_t* mysens = datamgr_get_sensor_by_id(mylist, (dummy)->id);
			if(mysens == NULL){
				char buffer[70];
				sprintf(buffer, "Sensor with id %d could not be mapped to a room\n", (dummy)->id);
				write_to_fifo(buffer);
			}else{
				datamgr_add_value_to_sensor((dummy)->value, (dummy)->ts, mysens);
			}
			queue_dequeue(queue);
		}
	}
	mylist_free(&mylist);
}
