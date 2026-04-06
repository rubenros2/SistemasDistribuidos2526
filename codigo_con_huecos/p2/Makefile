# EPI (Gijon)
# Grado en Ingenieria Informatica
# Sistemas de Distribuidos (Curso 3)
# Aplicaciones Distribuidas en Linux/Unix con Sun RPC


#****************************************************
# Variables
#****************************************************


CC=gcc
RPCGEN=rpcgen
CFLAGS= -I. -Wall -ggdb -I/usr/include/tirpc
LDFLAGS= -ltirpc -lpthread
CLIENTE = cliente
SERVIDOR= servidor

todo : $(CLIENTE) $(SERVIDOR)

#**************************************************
# Generacion del cliente y del servidor
#**************************************************

$(CLIENTE) : cliente.o srvwww_clnt.o srvwww_xdr.o util.o
	$(CC) $(CFLAGS) -o $(CLIENTE) cliente.o srvwww_clnt.o srvwww_xdr.o util.o $(LDFLAGS)


$(SERVIDOR) : srvwww_svc.o srvwww_xdr.o util.o servicios.o
	$(CC) $(CFLAGS) -o $(SERVIDOR) srvwww_svc.o srvwww_xdr.o util.o servicios.o $(LDFLAGS)

srvwww_svc.o : srvwww_svc.c srvwww.h
	$(CC) $(CFLAGS) -c srvwww_svc.c 

servicios.o: servicios.c srvwww.h util.h
	$(CC) $(CFLAGS) -c servicios.c

srvwww_clnt.o : srvwww_clnt.c srvwww.h
	$(CC) $(CFLAGS) -c srvwww_clnt.c

srvwww_xdr.o : srvwww_xdr.c srvwww.h
	$(CC) $(CFLAGS) -c srvwww_xdr.c

cliente.o : cliente.c srvwww.h util.h 
	$(CC) $(CFLAGS) -c cliente.c

util.o : util.c util.h srvwww.h
	$(CC) $(CFLAGS) -c util.c

srvwww.h srvwww_svc.c: srvwww.x
	$(RPCGEN) $<
	python3 apply_srvwww_svc_patches.py

#**************************************************
# Utilidades
#**************************************************

limpia : 
	rm -f *.o	

cleanall: limpia
	rm -f srvwww_*
	rm -f srvwww.h
	rm -f cliente servidor
