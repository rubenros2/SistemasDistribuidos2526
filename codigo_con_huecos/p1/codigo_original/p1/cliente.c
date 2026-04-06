
// Archivos de cabecera para manipulación de sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include "protocolo.h"
#include "util.h"

//
// VARIABLES GLOBALES
//

// IP del proceso srvwww
char *ip_srvwww;

// Puerto en el que espera el proceso srvwww
int puerto_srvwww;

// Numero de hilos lectores
int nhilos;

// nombre del fichero fuente de peticiones
char *fich_peticiones;

// nombre del fichero de log
char *fich_log;

// Tamaño máximo de los trozos descargados por UDP
int chunk_max_size;

// handlers de archivo
FILE *fppet;
FILE *fplog;

// mutex de exclusión al fichero de peticiones y de log
pthread_mutex_t mutex_fppet = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_fplog = PTHREAD_MUTEX_INITIALIZER;

void procesa_argumentos(int argc, char *argv[])
{
    if (argc < 7)
    {
        fprintf(stderr, "Forma de uso: %s ip_srvwww puerto_srvwww fich_pet nhilos fich_log chunk_max_size\n", argv[0]);
        exit(1);
    }
    ip_srvwww = strdup(argv[1]);
    if (!valida_ip(argv[1]))
    {
        fprintf(stderr, "Error: La IP introducida no es valida\n");
        exit(2);
    }
    if (!valida_numero(argv[2]))
    {
        fprintf(stderr, "Error: El valor de puerto no es un numero\n");
        exit(3);
    }
    puerto_srvwww = atoi(argv[2]);
    if ((puerto_srvwww < 1024) || (puerto_srvwww > 65535))
    {
        fprintf(stderr, "Error: El puerto debe ser mayor o igual a 1024 y menor o igual que 65535\n");
        exit(4);
    }
    fich_peticiones = argv[3];
    if (!valida_numero(argv[4]))
    {
        fprintf(stderr, "Error: El parámetro de número de hilos no es un número\n");
        exit(6);
    }
    nhilos = atoi(argv[4]);
    if (nhilos < 1)
    {
        fprintf(stderr, "Error: El numero de hilos cliente debe ser mayor o igual a 1\n");
        exit(7);
    }
    fich_log = argv[5];

    if (!valida_numero(argv[6]))
    {
        fprintf(stderr, "Error: El parámetro de tamaño máximo de chunk no es un número\n");
        exit(8);
    }
    chunk_max_size = atoi(argv[6]);
    if (chunk_max_size < 1)
    {
        fprintf(stderr, "Error: El tamaño máximo de chunk debe ser mayor o igual a 1\n");
        exit(9);
    }
}

void print_log(const char *format, ...)
{
    // Función auxiliar para imprimir mensajes en el fichero de log
    //
    // Se usa igual que printf, por lo que recibe un número variable de argumentos
    // y una cadena de formato. Por ejemplo:
    //
    // print_log("Error: No se pudo abrir el fichero %s\n", nombre_fichero);

    char buffer[TAMMSG];
    va_list args;

    // El siguiente trozo accede a la cadena de formato recibida y a la lista
    // de argumentos que vayan detrás, y le pasa todo a vsnprintf para que
    // formatee la cadena con los parámetros adecuadamente.
    // El resultado (que queda almacenado en buffer) es el mensaje que
    // queremos enviar finalmente al fichero de log
    va_start(args, format);
    vsnprintf(buffer, TAMMSG, format, args);
    va_end(args);

    // Escribimos el mensaje en buffer en el fichero fplog, protegiendo
    // el acceso con el mutex apropiado.
	// A RELLENAR
	...
	...
}

void salir_bien(int s)
{
    exit(0);
}

char * enviar_peticion(char *peticion, struct sockaddr_in *d_serv, int es_stream)
{
    /*
        Esta función envía un mensaje (en el parámetro peticion) al servidor, usando
        TCP o UDP según el valor del parámetro es_stream. La IP y puerto del servidor
        vienen en el parámetro d_serv.

        Una vez enviada la petición se espera a recibir una respuesta y se retorna
        dicha respuesta en un puntero a cadena, reservada dinámicamente (con strndup)

        Para las comunicaciones de red hace uso de las funciones en protocolo.c

        Quien llame a esta función debe liberar la memoria del buffer devuelto una
        vez que haya terminado de usarlo.
    */

    int sock_dat;
    int enviados, recibidos;
    
    char respuesta[TAMMAXRESPUESTA];
    char *s;

    bzero(respuesta, TAMMAXRESPUESTA);
    if (es_stream)
    {
        // A RELLENAR
        ...
        ...
    }
    else // se envía la petición por socket UDP
    {
		// A RELLENAR
		...
		...
    }
    close(sock_dat);
    s = (char *)malloc(TAMMAXRESPUESTA * sizeof(char));
    if (s == NULL)
    {
        print_log("Error al enviar peticion: No se pudo reservar memoria para hacer un duplicado de la respuesta\n");
        exit(15);
    }
	// A RELLENAR
	...
	...
}

void descarga_multimedia(struct sockaddr_in *p, char *fichero)
{
    /*
        Esta función se encarga de descargar las imágenes referenciadas dentro del fichero
        especificado en el parámetro fichero. Las imágenes serán descargadas utilizando
        el protocolo UDP en trozos de tamaño máximo chunk_max_size.
    */
    // Implementación de la descarga de imágenes (no proporcionada en el código original)
    FILE *fpin;              // para escribir en el fichero de salida
    int fdout;              // para escribir en el fichero de salida
    char linea[TAMLINEA];   // para almacenar la linea leida del fichero 
    char peticion[TAMMSG];  // para construir las peticiones a enviar
    char *s;               // para almacenar la respuesta del servidor
    long int tamfichero;   // para almacenar el tamaño del fichero a descargar
    long int offset;       // para almacenar el offset de un fichero
    char *pos;
    char *token;

    // Preparar peticion para obtener el tamaño del fichero, y enviarla
    // Abrimos el fichero para leerlo

    if ((fpin = fopen(fichero, "r")) == NULL)
    {
        print_log("Error: No se pudo abrir el fichero %s para leer\n", fichero);
        exit(18);
    }
    // Leer el fichero línea a línea    
    while (fgets(linea, TAMLINEA, fpin) != NULL)
    {
        pos= strstr(linea,"<img ");
        if (pos == NULL)
        {
           pos = strstr(linea,"<IMG ");
        }
        if (pos != NULL)  // hay una referencia a una imagen dentro de la linea
        {
            pos += 5; // saltamos "<img " o "<IMG "
            token=NULL;
            // extraemos la cadena hasta encontrar el terminador de la etiqueta
            token = strtok_r(pos, ">\n\r ", &s);
            if (token != NULL)
            {
               sprintf(peticion, "EXISTFILE %s", token);
               s = enviar_peticion(peticion, p, FALSO);
               if (strcmp(s, "200") == 0)
               {
                 free(s);
                 sprintf(peticion, "GETSIZE %s", token);
                 s = enviar_peticion(peticion, p, FALSO);
                 if (s!=NULL)
                 {
                    sscanf(s, "%ld", &tamfichero);
                    free(s);
                    if (tamfichero >= 0)
                    {
                        // Abrir fichero localmente para escribir en él lo que se reciba
                        fdout = open(token, O_WRONLY | O_CREAT, 0644); // Permisos rw-r--r--
                        if (fdout < 0)
                        {
                            print_log("Error: No se pudo abrir el fichero %s para escribir\n", token);
                            exit(21); 
                        }
                        // Iterar por los chunks a recibir
                        offset = 0; // Offset inicial
                        while (offset < tamfichero)
                        {
                            // Preparar peticion para obtener el chunk, y enviarla
                            if ((offset+chunk_max_size) > tamfichero)
                                chunk_max_size = tamfichero - offset; // Ajustar el tamaño del chunk al final del fichero
                            sprintf(peticion, "GETCHUNK %s %ld %d", token, offset, chunk_max_size);
                            s = enviar_peticion(peticion, p, FALSO);
                            if (s != NULL)
                            {
                                // Escribir en el fichero el chunk recibido
                                write(fdout, s, chunk_max_size);
                                free(s);
                                offset += chunk_max_size; // Incrementar el offset
                            }
                            else // Manejo de error al obtener el trozo
                            {
                                print_log("Error: No se pudo obtener el chunk del fichero %s\n", token);
                                close(fdout);
                                exit(23);
                            }
                        }
                        close(fdout);
                    }
                    else
                    {
                        print_log("Error: El tamaño del fichero %s no puede ser negativo\n", token);
                    }     
                 }
                 else // Manejo de error al obtener el tamaño del fichero
                 {                    
                    print_log("Error: No se pudo obtener el tamaño del fichero %s\n", token);
                 }
               }
               else
               {
                   free(s);
                   print_log("Error: El fichero %s no existe en el servidor\n", token);
               }
            }
        }
    }
    fclose(fpin);
}   

void *hilo_descarga(struct sockaddr_in *p)
{
    int recibidos;           // para almacenar el número de bytes recibidos
    long int tamfichero;          // para almacenar el tamaño del fichero a descargar
    char peticion[TAMMSG];   // para construir las peticiones a enviar
    char buffer[TAMMAXRESPUESTA];   // para leer una línea del fichero de peticiones
    char *s;                 // para almacenar la respuesta del servidor
    int  fdout;              // para escribir en el fichero de salida
    char *pt = NULL;         // para separar los tokens de la línea leída
    char *loc;               // para poder usar strtok_r 
    char *nomfile = NULL;    // para almacenar el nombre del fichero
    unsigned char es_stream; // para indicar si la petición se hace por TCP o UDP
    long int offset;              // para almacenar el offset de un fichero

    // Coger cerrojo antes de leer una línea del fichero de peticiones
    pthread_mutex_lock(&mutex_fppet);
    fprintf(stderr, "Hilo lector antes de entrar en el bucle\n");
    while (fgets(buffer, TAMMAXRESPUESTA, fppet) != NULL)
    {
        // Soltarlo tras haber leido la línea
        pthread_mutex_unlock(&mutex_fppet);

        // quitamos el salto de linea
        // la linea leida es de la forma "nombre_fichero T\n" o "nombre_fichero U\n"
        buffer[strlen(buffer) - 1] = '\0';

        // Separamos en tokens, siendo posibles separadores el espacio y el salto de linea
        pt = strtok_r(buffer, " \n", &loc);
        
        // Primer token, nombre de fichero
        if (pt == NULL)
        {
            print_log("Error: No se pudo obtener el nombre del fichero de la peticion\n");
            exit(16);
        }
        nomfile = strdup(pt);
        if (nomfile == NULL)
        {
            print_log("Error: No se pudo reservar memoria para el nombre del fichero de la peticion\n");
            exit(17);
        }
        // Segundo token, "T" o "U"
        pt = strtok_r(NULL, " \n", &loc);
        // Asignar apropiadamente es_stream
		// A RELLENAR
		...
		...
        fprintf(stderr, "Hilo lector antes de enviar peticion sobre fichero %s\n", nomfile);

        // Preparar peticion para ver si existe el fichero, y enviarla
		// A RELLENAR
		...
		...
        if (strcmp(s, "200") == 0)
        {
            free(s);
            // En funcion de si la descarga se realiza por TCP o UDP, se procederá
			// A RELLENAR
			...
			...
            }
            else // la descarga se realiza por UDP
            {
                // Preparar peticion para obtener el tamaño del fichero, y enviarla 
                // A RELLENAR
                ...
                ...
                if (s != NULL)
                {
                    // Extraer el tamaño del fichero de la respuesta
                    sscanf(s, "%ld", &tamfichero);
                    free(s);
                    if (tamfichero >= 0)
                    {
                        // Abrir fichero localmente para escribir en él lo que se reciba
                        fdout = open(nomfile, O_WRONLY | O_CREAT, 0644); // Permisos rw-r--r--
                        if (fdout < 0)
                        {
                            print_log("Error: No se pudo abrir el fichero %s para escribir\n", nomfile);
                            exit(20); 
                        }
                        // Iterar por los chunks a recibir
                        offset = 0; // Offset inicial
                        while (offset < tamfichero)
                        {
                            // Preparar peticion para obtener el chunk, y enviarla
                            // A RELLENAR
                            ...
                            ...
                            if (s != NULL)
                            {
                                // Escribir en el fichero el chunk recibido
                                write(fdout, s, chunk_max_size);
                                free(s);
                                offset += chunk_max_size; // Incrementar el offset
                            }
                            else // Manejo de error al obtener el trozo
                            {
                                print_log("Error: No se pudo obtener el chunk del fichero %s\n", nomfile);
                                close(fdout);
                                exit(22);
                            }
                        }
                        close(fdout);
                        if (tamfichero > 0)
                        {
                            // Llama a la función para descargar las imagenes referenciadas dentro del fichero
                            descarga_multimedia(p,nomfile); 
                        }
                    }
                    else // Si tamfichero es negativo
                    {
                        print_log("Error: El tamaño del fichero %s no puede ser negativo\n", nomfile);
                    }
                }
                else // Manejo de error al obtener el tamaño del fichero
                {
                    print_log("Error: No se pudo obtener el tamaño del fichero %s\n", nomfile);
                }
            }
        }
        else // 
        {
            free(s);
            print_log("Error: El fichero %s no existe en el servidor\n", nomfile);
        }
        fprintf(stderr, "Hilo lector antes de hacer mutex lock de mutex_fppet\n");
        if (nomfile != NULL) free(nomfile);
         // Coger cerrojo para la siguiente iteración del bucle
        pthread_mutex_lock(&mutex_fppet);
    }
    // Soltarlo tras la última iteración
    pthread_mutex_unlock(&mutex_fppet);
}

void main(int argc, char *argv[])
{
    register int i;             // Para iterar en bucles
    pthread_t *th;              // array dinámico de identificadores de hilo
    struct sockaddr_in d_serv;  // Para contener IP-puerto del servidor

    signal(SIGINT, salir_bien);
    procesa_argumentos(argc, argv);

    // Se abren los ficheros de peticiones y de log
    // y las correspondientes variables globales serán usadas desde los hilos
    if ((fppet = fopen(fich_peticiones, "r")) == NULL)
    {
        perror("Error: No se pudo abrir el fichero de peticiones");
        exit(8);
    }

    if ((fplog = fopen(fich_log, "w")) == NULL)
    {
        perror("Error: No se pudo abrir el fichero de log");
        exit(9);
    }
    setbuf(fplog, NULL); // Desactivar el buffer del fichero de log

    // Reserva dinámica de memoria para el array de identificadores de hilo
	// A RELLENAR
	...
	...
    // Configuración de la estructura con la IP y puerto del servidor
    d_serv.sin_family = AF_INET;
    d_serv.sin_addr.s_addr = inet_addr(ip_srvwww);
    d_serv.sin_port = htons(puerto_srvwww);

    // Creación de los hilos de descarga
	// A RELLENAR
	...
	...
    // Espera a que todos los hilos terminen
    for (i = 0; i < nhilos; i++)
    {
        pthread_join(th[i], NULL);
    }
}
