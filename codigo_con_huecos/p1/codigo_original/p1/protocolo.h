#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifndef PROTOCOLO_H
#define PROTOCOLO_H

int CrearSocketServidorTCP(int puerto);
int CrearSocketUDP(int puerto);
int AceptarConexion(int sock);
int CrearSocketDatosTCP();
int CrearSocketDatosUDP();
int Conectar(int sock, char *ip, int puerto);
int sendStringTCP(int s, char *str, int len);
int sendStringUDP(int s, char *str, int len, struct sockaddr_in *dir);
int recibirStringTCP(int s, char *str, int len);
int recibirStringUDP(int s, char *str, int len, struct sockaddr_in *dir);
void CerrarSocket(int s);

#endif
