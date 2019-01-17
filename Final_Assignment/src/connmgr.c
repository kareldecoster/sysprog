#include "../include/connmgr.h"

extern int active;
MYSQL* con;

void connmgr_run(){

	con = init_connection('0');
	if(con != NULL) {
		write_to_fifo("Connection to My_SQL server established.\n");
		sensor_data_ptr_t* prev = NULL;
		while(active){	
			int result = 0;
			sensor_data_ptr_t* data = (sensor_data_ptr_t*)queue_top(queue);
			if( (*data )!= NULL && data!=prev){
				sensor_data_ptr_t dummy = *data;
				prev = data;
				result = insert_sensor(con, (dummy)->id, (dummy)->value, (dummy)->ts);
				if(result < 0 ){
					char buff[70];
					sprintf(buff,"Failed to upload measurement where sensor-id = %d , temperature = %lf , time = %ld\n",(dummy)->id, (dummy)->value, (dummy)->ts);
					write_to_fifo(buff);
				}
				queue_dequeue(queue);

			}
		}
		disconnect(con);
		write_to_fifo("Disconnected from the My_SQL database.\n");
		con = NULL;
	}else{
		write_to_fifo("Unable to connect to My_SQL server.\n");	
	}
	mysql_library_end();


}

