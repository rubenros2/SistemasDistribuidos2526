#include "protocolo.h"

int CrearSocketServidorTCP(int puerto)
{
    // Crea un socket pasivo en el puerto dado y lo retorna
    // Aborta la ejecución del programa en caso de error

    int ret;
    int sock;
    struct sockaddr_in dir;

    ret = socket(PF_INET, SOCK_STREAM, 0);
    if (ret == -1)
    {
        perror("Al crear socket TCP");
        exit(20);
    }
    sock = ret;
    if (puerto != 0)
    {
        dir.sin_family = AF_INET;
        dir.sin_port = htons(puerto);
        dir.sin_addr.s_addr = htonl(INADDR_ANY);
        ret = bind(sock, (struct sockaddr *)&dir, sizeof(dir));
        if (ret == -1)
        {
            perror("Al asignar direccion al socket TCP");
            close(sock);
            exit(21);
        }
    }
    ret = listen(sock, SOMAXCONN);
    if (ret == -1)
    {
        perror("Al poner en modo escucha el socket TCP");
        close(sock);
        exit(22);
    }
    return sock;
}

int CrearSocketUDP(int puerto)
{
    // Crea un socket pasivo en el puerto dado y lo retorna
    // Aborta la ejecución del programa en caso de error

    int ret;
    int sock;
    struct sockaddr_in dir;

    ret = socket(PF_INET, SOCK_DGRAM, 0);
    if (ret == -1)
    {
        perror("Al crear socket UDP");
        exit(20);
    }
    sock = ret;
    if (puerto != 0)
    {
        dir.sin_family = AF_INET;
        dir.sin_port = htons(puerto);
        dir.sin_addr.s_addr = htonl(INADDR_ANY);
        ret = bind(sock, (struct sockaddr *)&dir, sizeof(dir));
        if (ret == -1)
        {
            perror("Al asignar direccion al socket UDP");
            close(sock);
            exit(21);
        }
    }
    return sock;
}

int AceptarConexion(int sock)
{
    // Acepta una conexión en el socket pasivo, o aborta la ejecución en caso de error
    int ret;

    ret = accept(sock, NULL, NULL);
    if (ret == -1)
    {
        perror("Al aceptar conexión en socket TCP");
        exit(22);
    }
    return ret;
}

int CrearSocketDatosTCP()
{
    // Crea y retorna un socket de datos
    // Aborta la ejecución en caso de error
    int sock;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Al crear socket de datos TCP");
        exit(23);
    }
    return sock;
}

int CrearSocketDatosUDP()
{
    // Crea y retorna un socket de datos
    // Aborta la ejecución en caso de error
    int sock;

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("Al crear socket de datos UDP");
        exit(23);
    }
    return sock;
}

int Conectar(int sock, char *ip, int puerto)
{
    // Intenta una conexión a la IP y puerto dados (la IP se espera como una
    // cadena de caracteres, ej: "127.0.0.1")
    // Aborta la ejecución del programa en caso de error

    struct sockaddr_in dir_serv;
    int result;

    // Inicializar dir_serv
	// A RELLENAR
	...
	...
    result = connect(sock, (struct sockaddr *)&dir_serv, sizeof(dir_serv));
    if (result == -1)
    {
        perror("Al conectar al servidor TCP");
        exit(25);
    }
    return result;
}

int sendStringTCP(int s, char *str, int len)
{
    int enviados;
    enviados = send(s, str, len, 0);
    if (enviados != len)
    {
        perror("Al enviar cadena por socket TCP");
    }
    return enviados;
}

int sendStringUDP(int s, char *str, int len, struct sockaddr_in *dir)
{
    int enviados;
    enviados = sendto(s, str, len , 0, (struct sockaddr *)dir, sizeof(struct sockaddr_in));
    if (enviados != len)
    {
        perror("Al enviar cadena por socket UDP");
    }
    return enviados;
}

int recibirStringTCP(int s, char *str, int len)
{
    int recibidos;
    recibidos = recv(s, str, len, 0);
    if (recibidos == -1)
    {
        perror("Al recibir cadena por socket TCP");
    }
    else
    {
		// A RELLENAR
		...
		...
    }
    return recibidos;
}

int recibirStringUDP(int s, char *str, int len, struct sockaddr_in *dir)
{
    int recibidos;
    int tam = sizeof(struct sockaddr_in);

    recibidos = recvfrom(s, str, len, 0, (struct sockaddr *)dir, &tam);
    if (recibidos == -1)
    {
        perror("Al recibir cadena por socket UDP");
    }
    else
    {
		// A RELLENAR
		...
		...
    }
    return recibidos;
}

void CerrarSocket(int s)
{
    close(s);
}