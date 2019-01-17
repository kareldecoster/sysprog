#include "../include/select_server.h"

#ifndef SET_TIMEOUT
#error PLEASE DEFINE SET_TIMEOUT AT COMPILE-TIME
#endif


#define PORT 1234
int mylist_errno;
int sock;
long timeout = SET_TIMEOUT;
struct timeval tv;
mylist_ptr_t mylist;
extern int active;

struct socketdata{
	int fd;
	int sensor_id;
	time_t ts;
};
typedef struct socketdata socketdata_t;

void element_copy(element_ptr_t *dest_element, element_ptr_t src_element){
	*dest_element = src_element;
}
void element_free(element_ptr_t *element){
	free(*element);
	*element = NULL;
}
int element_compare(element_ptr_t x, element_ptr_t y){
	socketdata_t* a = (socketdata_t*)x;
	socketdata_t* b = (socketdata_t*)y;
	if((a->fd) == (b->fd)){
		return 0;
	}
	if((a->fd)<(b->fd)){
		return -1;
	}else{
		return 1;
	}
}
void element_print(element_ptr_t element){
	printf("Socket fd: %d sensor-id: %d\n",(((socketdata_t*)element)->fd), (((socketdata_t*)element)->sensor_id));

}


int make_socket (uint16_t port){
	int sock;
	struct sockaddr_in name;

	/* Create the socket. */
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror ("socket");
		mylist_free(&mylist);
		exit (EXIT_FAILURE);
	}

	/* Give the socket a name. */
	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){
		perror("setsockopt(SO_REUSEADDR) failed");
	}
	if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
	{
		perror ("bind");
		mylist_free(&mylist);
		exit (EXIT_FAILURE);
	}

	return sock;
}


int handle_connection(socketdata_t* sd){
	int bytes;
	uint16_t sensor_id;
 	double temperature;
 	time_t timestamp;
	bytes = read( sd->fd, (void *)&sensor_id, sizeof(sensor_id));
	// bytes == 0 indicates tcp connection teardown
	assert( (bytes == sizeof(sensor_id)) || (bytes == 0) );	
	bytes = read( sd->fd, (void *)&temperature, sizeof(temperature));
	assert( (bytes == sizeof(temperature)) || (bytes == 0) );
	bytes = read( sd->fd, (void *)&timestamp, sizeof(timestamp));
	assert( (bytes == sizeof(timestamp)) || (bytes == 0) );    
	if ( bytes ) 
	{	
		if(sd->sensor_id == -1){
			sd->sensor_id = sensor_id;
			char buffer[70];
			sprintf(buffer, "A sensor node with sensor-id %d has opened a new connection.\n", sensor_id);
			write_to_fifo(buffer);
		}
		sensor_data_ptr_t dummy = malloc(sizeof(sensor_data_t));
		dummy->id = sensor_id;
		dummy->value = temperature;
		dummy->ts = timestamp;
		queue_enqueue(queue,(void*)dummy);
	}else{
		return -1;
	}
	return 0;
	//sleep(1);	/* to allow socket to drain */
}

int server_run( void )
{
	fd_set active_fd_set, read_fd_set;
	int i, j;
	size_t size;
	struct sockaddr_in clientname;
	mylist = mylist_create(&element_copy, &element_free, &element_compare, &element_print);
	assert(mylist!=NULL);
	sock = make_socket(PORT); //Create a socket and set it up to accept connections.
	tv.tv_sec = SET_TIMEOUT / 2;
	tv.tv_usec = 0;
	if ( listen (sock, 1) < 0 ){
		perror("listen");
		exit(EXIT_FAILURE);
	
	}
	FD_ZERO(&active_fd_set);	//Initialize the set of active sockets.
	FD_SET(sock,&active_fd_set);
	
	while (active){
		read_fd_set = active_fd_set;
		if( select(FD_SETSIZE+1, &read_fd_set, NULL, NULL, &tv) < 0 )	//Block untill input arrives on one of the active sockets or untill timeout time is reached
		{
			perror("select");
			exit(EXIT_FAILURE);
		}
		//Handle all the sockets with pending input.
		for( i = 0; i < FD_SETSIZE; ++i ){
			if( FD_ISSET(i, &read_fd_set) ){
				if( i == sock ){
					//Connection on the "server" socket
					int new;
					size = sizeof(clientname);
					new = accept(sock,
								(struct sockaddr*) &clientname,
								(socklen_t*) &size);
					if( new < 0 ) {
						perror("accept");
						exit(EXIT_FAILURE);
					}
					int optval = SET_TIMEOUT;
					socklen_t optlen = sizeof(optval);
					if(setsockopt (new, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen)<0){
						perror("setsockopt()");
						close(new);
					
					}
					char buffer[80];
					sprintf(buffer, "Server: connect from host %s, port %hu.\n",inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port));
					write_to_fifo(buffer);
					FD_SET (new, &active_fd_set);
					socketdata_t* sd = malloc(sizeof(socketdata_t));
					sd->fd = new; 
					sd->sensor_id = -1;
					sd->ts = time(NULL);
					mylist_insert_sorted(mylist, (void*)sd);
				}else{
					//Data arriving from an already-connected socket
					socketdata_t sd;
					socketdata_t* sdp;
					sd.fd = i;

					j = mylist_get_index_of_element(mylist, ((void*)&sd));
					sdp = (socketdata_t*)mylist_get_element_at_index(mylist, j);
					if(handle_connection(sdp)<0){
						close(i);						
						char buffer[70];
						sprintf(buffer, "The sensor node with sensor-id %d has closed the connection.\n", sdp->sensor_id);
						write_to_fifo(buffer);
						mylist_free_at_index(mylist, j);
						FD_CLR(i, &active_fd_set);
					}else{
						sdp->ts = time(NULL);
					}
				}
			}
		}
		for(j=0;j<mylist_size(mylist);j++){
			socketdata_t* dummy = (socketdata_t*)mylist_get_element_at_index(mylist,j);
			if(dummy!=NULL){
				if(dummy->ts + timeout < time(NULL)){
					char buffer[70];
					sprintf(buffer,"The sensor node with sensor id %d has disconnected due to timeout.\n", dummy->sensor_id);
					write_to_fifo(buffer);
					FD_CLR(dummy->fd, &active_fd_set);
					FD_CLR(dummy->fd, &read_fd_set);
					close(dummy->fd);
					mylist_free_at_index(mylist,j);
					j--;
				}
			}
		}	
	}
	//At the end of the endless loop we need to free some variables
	mylist_free(&mylist);
	close(sock);
	return 0;
}


