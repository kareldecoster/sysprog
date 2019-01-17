#include <my_global.h>
#include <time.h>
#include "../include/sensor_db.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


	
void close_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);

}


/*
 * Make a connection to MySQL database
 * Create a table named 'yourname' if the table does not exist
 * If the table existed, clear up the existing data if clear_up_flag is set to 1
 * return the connection for success, NULL if an error occurs
 */

MYSQL *init_connection(char clear_up_flag){
	MYSQL *con = mysql_init(NULL);

	if (con == NULL) 
	{
		close_with_error(con);
		return NULL;
	
	}

	if (mysql_real_connect(con, "studev.groept.be", "a13_syssoft", "a13_syssoft", 
			"a13_syssoft",3306 , NULL, 0) == NULL) 
	{
		close_with_error(con);
		return NULL;
	
	}  
	if (mysql_query(con, "CREATE TABLE IF NOT EXISTS KarelDeCoster(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, sensor_id INT, sensor_value DECIMAL(4,2), 							timestamp TIMESTAMP)")) {			
			close_with_error(con);
 	  		return NULL;
	
	}
	if(clear_up_flag == '1'){
		if(mysql_query(con, "TRUNCATE TABLE KarelDeCoster")){
			printf("CLEAR UP FAILED");
			return NULL;
		}
	  	printf("CLEAR UP \n");
	  	
	}
	return con;
	
}

/*
 * Disconnect MySQL database
 */
void disconnect(MYSQL *conn){
	mysql_close(conn);
	conn = NULL;

}

/*
 * Write an INSERT query to insert a single sensor measurement
 * return zero for success, and non-zero if an error occurs
 */

int insert_sensor(MYSQL *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	char str[180] ;
	char buff[20] ;
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&ts));
	sprintf(str, "INSERT INTO KarelDeCoster (sensor_id, sensor_value, timestamp) VALUES(%d, %.2lf ,'%s')", id, value, buff);
	if (mysql_query(conn, str)) {
		return -1;
	
	}
	
	return 0;
	
}

/*
 * Write an INSERT query to insert all sensor measurements existed in the file pointed by sensor_data
 * return zero for success, and non-zero if an error occurs
 */
int insert_sensor_from_file(MYSQL *conn, FILE *sensor_data){
	uint16_t id;
	double temperature;
	time_t timestamp;
	int a,b,c;
	while(1){
		a = fread(&id, sizeof(uint16_t), 1, sensor_data);
		b = fread(&temperature, sizeof(double), 1, sensor_data);
		c = fread(&timestamp, sizeof(time_t), 1, sensor_data);
		if(a!=0 && b!= 0 && c!=0){
			if(insert_sensor(conn, id, temperature, timestamp)){
				return -1;
		
			}		
		}else{		
			return 0;
		
		}	
	}
	
}

/*
 * Write a SELECT query to return all sensor measurements existed in the table
 * return MYSQL_RES with all the results
 */

MYSQL_RES *find_sensor_all(MYSQL *conn){
	if (mysql_query(conn, "SELECT * FROM KarelDeCoster")) 
	  {
		  close_with_error(conn);
		  return NULL;
	  }
	  
	MYSQL_RES *result = mysql_store_result(conn);
  
	if (result == NULL) 
	{
		close_with_error(conn);
		return NULL;
	}
	return result;

}

/*
 * Write a SELECT query to return all sensor measurements containing 'value_t'
 * return MYSQL_RES with all the results
 */

MYSQL_RES *find_sensor_by_value(MYSQL *conn, sensor_value_t value_t){
	char query[50];
	sprintf(query, "SELECT * FROM KarelDeCoster where sensor_value=%.2lf", value_t);
	if (mysql_query(conn, query)) 
	  {
		  close_with_error(conn);
		  return NULL;
	  }
	  
	MYSQL_RES *result = mysql_store_result(conn);
  
	if (result == NULL) 
	{
		close_with_error(conn);
		return NULL;
	}
	return result;

}

/*
 * Write a SELECT query to return all sensor measurement that its value exceeds 'value_t'
 * return MYSQL_RES with all the results
 */
MYSQL_RES *find_sensor_exceed_value(MYSQL *conn, sensor_value_t value_t){
	char query[50];
	sprintf(query, "SELECT * FROM KarelDeCoster where sensor_value>%.2lf", value_t);
	if (mysql_query(conn, query)) 
	  {
		  close_with_error(conn);
		  return NULL;
	  }
	  
	MYSQL_RES *result = mysql_store_result(conn);
  
	if (result == NULL) 
	{
		close_with_error(conn);
		return NULL;
	}
	return result;

}

/*
 * Write a SELECT query to return all sensor measurement containing timestamp 'ts_t'
 * return MYSQL_RES with all the results
 */
MYSQL_RES *find_sensor_by_timestamp(MYSQL *conn, sensor_ts_t ts_t){
	char query[50];
	char buff[20];
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&ts_t));
	sprintf(query, "SELECT * FROM KarelDeCoster where timestamp=%s", buff);
	if (mysql_query(conn, query)) 
	  {
		  close_with_error(conn);
		  return NULL;
	  }
	  
	MYSQL_RES *result = mysql_store_result(conn);
  
	if (result == NULL) 
	{
		close_with_error(conn);
		return NULL;
	}
	return result;

}

/*
 * Write a SELECT query to return all sensor measurement recorded later than timestamp 'ts_t'
 * return MYSQL_RES with all the results
 */
MYSQL_RES *find_sensor_later_timestamp(MYSQL *conn, sensor_ts_t ts_t){
	char query[50];
	char buff[20];
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&ts_t));
	sprintf(query, "SELECT * FROM KarelDeCoster where timestamp>%s", buff);
	if (mysql_query(conn, query)) 
	  {
		  close_with_error(conn);
		  return NULL;
	  }
	  
	MYSQL_RES *result = mysql_store_result(conn);
  
	if (result == NULL) 
	{
		close_with_error(conn);
		return NULL;
	}
	return result;

}

/*
 * Return the number of records contained in the result
 */
int get_result_size(MYSQL_RES *result){

	return mysql_num_fields(result);
	
}

/*
 * Print all the records contained in the result
 */
void print_result(MYSQL_RES *result){
	int num_fields = mysql_num_fields(result);

	  MYSQL_ROW row;
	  
	  while ((row = mysql_fetch_row(result))) 
	  { 
	  int i = 0;
		  while(i < num_fields)
		  { 
			  printf("%s ", row[i] ? row[i] : "NULL"); 
			  i++;
		  } 
			  printf("\n"); 
	  }
}
