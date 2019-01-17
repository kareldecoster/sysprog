#include "../include/sqlite_connmgr.h"
#include <unistd.h>


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int sqlite_connmgr_run(){
	sqlite3 *db;
	char *zErrMsg = 0;
	char* sql;
	int rc;

	rc = sqlite3_open("../res/sqlite_db", &db);

	if( rc ){
		char buff[100];
		sprintf(buff, "Can't open SQLite database: %s\n", sqlite3_errmsg(db));
		write_to_fifo(buff);
		exit(0);
	}else{
		write_to_fifo("Opened SQLite database successfully.\n");
	}
	   /* Create SQL statement */
	sql = "CREATE TABLE KarelDeCoster("  \
		 "id INTEGER PRIMARY KEY AUTOINCREMENT    NOT NULL," \
 		 "sensor_id      INT    NOT NULL," \
		 "sensor_value   REAL   NOT NULL," \
		 "timestamp      INT );";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		char buff[100];
		sprintf(buff, "SQLite error: %s\n", zErrMsg);
		write_to_fifo(buff);
		sqlite3_free(zErrMsg);
	}else{
		write_to_fifo("Table created successfully.\n");
	}
	sensor_data_ptr_t* curr;
	sensor_data_ptr_t* prev = NULL;
	
	//LOOP
	while(active){
		curr = (sensor_data_ptr_t*)queue_top(queue);
		if((*curr)!=NULL && curr!=prev){
			sensor_data_ptr_t dummy = *curr;
			prev = curr;
			char buff[100];
			sprintf(buff,"INSERT INTO KarelDeCoster (sensor_id,sensor_value,timestamp) "  \
	   			  "VALUES (%d, %lf, %ld); ", (dummy)->id, (dummy)->value, (dummy)->ts);
			rc = sqlite3_exec(db, buff, callback, 0, &zErrMsg);
			if( rc != SQLITE_OK ){
				sprintf(buff, "SQLite error: %s\n", zErrMsg);
				write_to_fifo(buff);
				sqlite3_free(zErrMsg);
			}
			queue_dequeue(queue);

		}
	}	
	
	//Terminate
	sqlite3_close(db);
	write_to_fifo("Disconnected from SQLite database.\n");
	return 0;
}
