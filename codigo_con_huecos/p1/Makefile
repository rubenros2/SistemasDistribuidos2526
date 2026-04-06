CC=gcc
CFLAGS=-I. -pthread -g
DEPS = cola.c util.c protocolo.c
OBJ = srvwww.o cola.o cliente.o util.o protocolo.o

all: srvwww cliente 

srvwww: srvwww.c $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) 

cliente: cliente.c util.c util.h protocolo.c protocolo.h
	$(CC) -o $@ $^ $(CFLAGS)  

limpia: 
	rm *.o
