IDIR = ../include
CFLAGS = -Wall -Werror -g -pthread -I$(IDIR)
CC = gcc

ODIR = obj
LDIR =../lib

_LIBS=mylistlib.so
LIBS = $(patsubst %,$(LDIR)/%,$(_LIBS))  -lsqlite3 

_DEPS = shared_queue.h connmgr.h config.h sensor_db.h select_server.h mylist.h datamgr.h sqlite_connmgr.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =  main.o shared_queue.o  connmgr.o sensor_db.o select_server.o datamgr.o sqlite_connmgr.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ1 = tcpsocket.o sensor_node.o #.o files
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))
	
all: $(OBJ)
	$ make gateway
	$ make sensor
	
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) `mysql_config --cflags --libs`
		
$(ODIR)/select_server.o: select_server.c ../include/select_server.h $(LIBS)
	$(CC) -c -o $@ $< $(CFLAGS) -D SET_TIMEOUT=60.00
	
$(ODIR)/datamgr.o: datamgr.c ../include/datamgr.h $(LIBS)
	$(CC) -c -o $@ $< $(CFLAGS) -D SET_MIN_TEMP=19.00 -D SET_MAX_TEMP=21.00

gateway: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) `mysql_config --cflags --libs` $(LIBS)
	
sensor: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)

mylistlib.so: mylist.c
	$(CC) -c mylist.c -Wall -Werror -fpic
	$ mv ./mylist.o ./obj/mylist.o
	$(CC) -shared -o mylistlib.so ./obj/mylist.o
	$ mv ./mylistlib.so ../lib/mylistlib.so
	
clean: 
	$ rm -f $(ODIR)/*.o core $(INCDIR)/*~
	$ rm -f gateway
	$ rm -f sensor
	$ rm my_fifo
	$ clear
	
	
.PHONY: clean
