#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "../include/config.h"
#include "../include/connmgr.h"
#include "../include/datamgr.h"
#include "../include/sqlite_connmgr.h"
#include "../include/select_server.h"
#include "../include/shared_queue.h"

//FUNCTION PROTOTYPES
void destroy_element(element_ptr_t*);
void copy_element(element_ptr_t*, element_ptr_t);
int compare_element(element_ptr_t, element_ptr_t);
void print_element(element_ptr_t);
void run_child (int);
void write_to_fifo(char*);
void* conn_thread_loop(void* );
void* data_thread_loop(void* );
void* server_thread_loop(void* );
void* sqlite_thread_loop(void* );
void sig_handler(int);
void child_sig_handler(int);

//GLOBAL VARIABLES
FILE* fifo;
FILE* fp;
queue_ptr_t queue;
long sequence_number = 0;
int active = 1;


int main(){
	pid_t child_pid; 
	int result;
	 
	result = mkfifo("my_fifo", 0666);
	if ( result == -1 )								
	{	
		if(errno != EEXIST){								
			perror("Error executing syscall");			
			exit( EXIT_FAILURE );	
		}					
	}	
	child_pid = fork();
	if ( child_pid == -1 )								
	{												
		perror("Error executing syscall");			
		exit( EXIT_FAILURE );						
	}
	  
	if ( child_pid == 0  )
	{  
		fifo = fopen("my_fifo", "r");
		run_child( 0 );
		return 0;
	}
	else
	{ 
		fifo = fopen("my_fifo", "w");
		//RUN PARENT HERE!
		signal(SIGINT, sig_handler);
		pthread_t connmgr;
		pthread_t datamgr;
		pthread_t server;
		pthread_t sqlite_connmgr;
		int amount_of_readers = 3;
		queue = queue_create(&destroy_element, &copy_element, &compare_element, &print_element, amount_of_readers);
		assert(queue!=NULL);
		
		//Start the threads
		pthread_create(&sqlite_connmgr, NULL, &sqlite_thread_loop, NULL);
		pthread_create(&connmgr, NULL, &conn_thread_loop, NULL);
		pthread_create(&datamgr, NULL, &data_thread_loop, NULL);
		pthread_create(&server, NULL, &server_thread_loop, NULL);
		
		
		//Wait for all your threads to close.
		pthread_join(server,NULL);
		pthread_join(datamgr,NULL);
		pthread_join(connmgr,NULL);
		pthread_join(sqlite_connmgr,NULL);
		
		//Only after all threads have stopped!
		queue_free(&queue);
		fclose(fifo);
		return 0;
	}
}

//LOOP FOR THE SQLITE CONNECTION MANAGER THREAD
void* sqlite_thread_loop(void* id){
	sqlite_connmgr_run();
	pthread_exit(NULL);
}

//LOOP FOR THE CONNECTION MANAGER THREAD
void* conn_thread_loop(void* id){
	connmgr_run();				//The loop for the connection manager, defined in connmgr.c
	pthread_exit(NULL);
}

//LOOP FOR THE SERVER THREAD
void* server_thread_loop(void* id){
	server_run();
	pthread_exit(NULL);
}

//LOOP FOR THE DATA MANAGER THREAD
void* data_thread_loop(void* id){
	datamgr_run();
	pthread_exit(NULL);
}

//LOOP FOR THE LOG PROCESS
void run_child ( int exit_code )
{
	signal(SIGINT, child_sig_handler);
	char* result;
	char recv_buf[70];
	fp = fopen("log", "w"); 
	do 
	{
		result = fgets(recv_buf, 70, fifo);
		if ( result != NULL )
	 	{ 
			fputs(recv_buf, fp);
	  	}
	} while ( result != NULL );
	fclose(fifo);
	fclose(fp);
	exit( exit_code ); // this terminates the child process
  
}



void write_to_fifo(char* message){
	char send_buf[100];
	char buff[20];
	time_t ltime; /* calendar time */
	ltime=time(NULL); /* get current cal time */
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&ltime));
	sprintf(send_buf,"%ld %s : %s",sequence_number++, buff, message);
	if ( fputs( send_buf, fifo ) == EOF ){
	  fprintf( stderr, "Error writing data to fifo\n");
	  exit( EXIT_FAILURE );
	} 
	fflush(fifo);
}

void destroy_element(element_ptr_t* element)  
{
	free(*element);
	*element = NULL;
}

void copy_element(element_ptr_t* dest_element, element_ptr_t src_element){
	*dest_element = src_element;
}

int compare_element(element_ptr_t x, element_ptr_t y){
	sensor_data_t* a = (sensor_data_t*) x;
	sensor_data_t* b = (sensor_data_t*) y;
  if(a->id == b->id){
  	return 0;
  }
  if((a->id) > (b->id)){
  	return 1;
  }else{
  	return -1;
  }
  
}

void print_element(element_ptr_t element){
	sensor_data_t* a = (sensor_data_t*) element;
	printf("Measurement --- Sensor id : %d --- temperature : %lf \n", (a->id),(a->value));
	
}

void sig_handler(int signum){
	active = 0; //This will end the infinite loops of the threads.
	printf("\nSHUTTING DOWN...\n");
}

void child_sig_handler(int signum){
	if(signum == SIGINT){
	//We dont want the child to get killed yet, it must exit when the fifo has no more writers to it.
	}
}
