/*
  Cliente de RPC que simula las operaciones de varios clientes del
  servidor www
*/
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <pthread.h>       
#include <fcntl.h>
#include "srvwww.h"
#include "util.h"

#define MAXHILOSCLIENTE 10

// Variables globales

// IP del proceso srvwww
char *ip_srv = NULL;

// numero de clientes
int num_clientes;

// tamaño maximo del trozo a descargar en cada invocación remota
int tam_chunk; 

// tipo de datos que recibiran los hilos cliente
struct datos_hilo
{
    unsigned char id_cliente;
    FILE *fpin;
};

typedef struct datos_hilo datos_hilo;

pthread_mutex_t m;    // mutex para serializar las invocaciones remotas
pthread_mutex_t mpet; // mutex para realizar la exclusión mutua sobre el fichero de peticiones

void grabar_lista_trozos(listatrozos *l, int fd)
{
    // Función que graba los trozos de la lista enlazada en el fichero fp
    listatrozos *aux = l;
    while (aux != NULL)
    {
        write(fd, aux->trozo.chunk_val, aux->trozo.chunk_len); // Grabar el trozo en el fichero 
        aux = aux->sgte;
    }
}

void descarga_fichero(int id_cliente, CLIENT *cl, char *nomfile, int chunk_max_size)
{
    Resultado *res; // Resultado devuelto por las RPC
    param1 q1;      // Parametro para las RPC que usen el tipo param1
    long tamfile;   // Tamaño del fichero a descargar
    long offset;    // Offset desde el que se va a descargar el fichero
    int fdout;      // Fichero de salida donde se va a descargar el fichero

	// A RELLENAR
	...
	...

    if (...)   // A RELLENAR
    {
        // Si el fichero existe, se continua preguntando su tamaño
        fprintf(stderr, "Cliente: %d El fichero %s existe en el servidor.\n", id_cliente, nomfile);
		// A RELLENAR
        ...
        ...

        if (res->caso == 1 && res->Resultado_u.val >= 0)
        {
            tamfile = res->Resultado_u.val; // Tamaño del fichero a descargar
            fprintf(stderr, "Cliente: %d El fichero %s tiene un tamaño de %ld bytes.\n", id_cliente, nomfile, res->Resultado_u.val);
            // Si el tamaño del fichero es positivo, se procede a descargarlo
            // Se crea el fichero de salida con el nombre del fichero a descargar
            if ((fdout = open(nomfile, O_WRONLY | O_CREAT , 0644)) < 0)
            {
                // Si no se pudo abrir el fichero de salida, se informa y se sigue con el siguiente fichero
                fprintf(stderr, "Cliente: %d No se pudo abrir el fichero de salida %s para escribir.\n", id_cliente, nomfile);
                free(nomfile);
                nomfile = NULL;
            }
            else
            {
                fprintf(stderr, "Cliente: %d Abre el fichero de salida %s para escribir.\n", id_cliente, nomfile);
                // Se descarga el fichero en trozos de tamaño tam_chunk
                offset = 0; // Offset inicial
                while (offset < tamfile)
                {
                    // Se prepara el parámetro para la RPC
                    q1.filename = nomfile;
                    q1.offset = offset;
                    q1.size = (tam_chunk < (tamfile - offset)) ? tam_chunk : (tamfile - offset);
                    fprintf(stderr, "Cliente: %d Pide al servidor un trozo del fichero %s a partir del offset %ld de tamaño %d bytes\n", id_cliente, nomfile, q1.offset, q1.size);
                    res = getchunk_1(&q1, cl);
                    if (res==NULL)
                    {
                        clnt_perror(cl, "Error en la invocación remota getchunk_1");
                        fprintf(stderr, "Cliente: %d Error al invocar la RPC getchunk_1 para el fichero %s\n", id_cliente, nomfile);
                        close(fdout); // Cerrar el fichero de salida
                        free(nomfile);
                        nomfile = NULL;
                        break;
                    }
                    else if (res->caso == 2) // Si la RPC se ha realizado correctamente
                    {
                        fprintf(stderr, "Cliente: %d Recibe un trozo del fichero %s de tamaño %d bytes\n", id_cliente, nomfile, q1.size);
                        // Se escribe el trozo en el fichero de salida
                        grabar_lista_trozos(res->Resultado_u.lista, fdout);
                        // Liberar la memoria de la lista de trozos
                        xdr_free((xdrproc_t) xdr_listatrozos, (char *) &res->Resultado_u.lista);
                        offset += q1.size; // Actualizar el offset
                    }
                    else if (res->caso==3)// Si hubo un error en la RPC
                    {
                        fprintf(stderr, "Cliente: %d Error al descargar el trozo del fichero %s: %s\n", id_cliente, nomfile, res->Resultado_u.err);
                        free(nomfile);
                        nomfile=NULL;
                        break;
                    }
                }
                close(fdout); // Cerrar el fichero de salida
                fprintf(stderr, "Cliente: %d Termina de descargar el fichero %s.\n", id_cliente, nomfile);
            }

        }
        else // Si el tamaño del fichero es negativo, o se produjo un ereror en la RPC, se informa y se sigue con el siguiente fichero
        {
            if (res->caso == 3)
            {
                fprintf(stderr, "Cliente: %d recibe mensaje de error: %s\n", id_cliente, res->Resultado_u.err);
            }
            else
            {
                fprintf(stderr, "Cliente: %d Intenta descargar un fichero %s que el servidor indica que tiene tamaño negativo.\n", id_cliente, nomfile);
            }
        }
    }
    else // Si el fichero no existe, o se produjo un error en la RPC, se informa y se sigue con el siguiente fichero
    {
        if (res->caso == 3)
        {
            fprintf(stderr, "Cliente: %d recibe mensaje de error: %s\n", id_cliente, res->Resultado_u.err);
        }
        else
        {
            fprintf(stderr, "Cliente: %d Intenta descargar un fichero %s que no existe en el servidor.\n", id_cliente, nomfile);
        }
    }
}

void procesar_etiquetas(int id_cliente, CLIENT *cl, char *fichero, int chunk_max_size)
{
    /*
        Esta función se encarga de descargar las imágenes referenciadas dentro del fichero
        especificado en el parámetro fichero. Las imagenes serán descargadas utilizando
        llamadas RPC al servidor. 
    */

    FILE *fpin;              // para leer el fichero de entrada
    char linea[TAMLINEA];   // para almacenar la linea leida del fichero 
    char *pos;
    char *s;               // para usar con strtok_r
    char *token;

    // Preparar peticion para obtener el tamaño del fichero, y enviarla
    // Abrimos el fichero para leerlo

    if ((fpin = fopen(fichero, "r")) == NULL)
    {
        fprintf(stderr,"Cliente %d Error: No se pudo abrir el fichero %s para leer\n",id_cliente, fichero);
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
                descarga_fichero(id_cliente, cl, token, chunk_max_size);                
            }
        }
    }
    fclose(fpin);
}   
// pensar en meter una funcion que sea descargar fichero que se llame desde
//procesarDescargas y desde descarga_multimedia

void *procesarDescargas(datos_hilo *p)
{
    CLIENT *cl;            // Estructura CLIENT para las RPC
    FILE *fppet;           // Fichero del que leer las peticiones a realizar
    int id_cliente;        // Numero de cliente (para mensajes de depuración)
    char buffer[TAMLINEA]; // Buffer de lectura de una línea del fichero de peticiones
    char msg[TAMLINEA];    // Buffer para mensajes de error para enviar a log_debug()
    char *token = NULL;    // Para tokenizar las líneas del fichero de peticiones
    char *loc;             // para usar con strtok_r
    char *nomfile = NULL;  // Nombre del fichero a descargar


    id_cliente = p->id_cliente; // Capturar el id del cliente en una variable local
    fppet = p->fpin;            // Capturar el puntero al fichero de peticiones en una variable local
    free(p);                    // Ya no necesitamos el parámetro recibido, lo liberamos

    bzero(buffer, TAMLINEA);

    pthread_mutex_lock(&mpet); // Bloquear el mutex que protege el fichero de peticiones
    while (fgets(buffer, TAMLINEA, fppet) != NULL)
    {
        fprintf(stderr, "Cliente: %d lee la peticion WWW: %s\n", id_cliente, buffer);
        pthread_mutex_unlock(&mpet); // desbloquear el mutex que protege el fichero de peticiones

        // Tokenizar la línea leida
        loc = NULL;
        token = strtok_r(buffer, " \n", &loc);
        if (token == NULL)
        {
            sprintf(msg, "Cliente: %d Error al extraer el primer token de la peticion WWW\n", id_cliente);
            log_debug(msg);
            exit(12);
        }

        // El primer token es el nombre del fichero
        nomfile = strdup(token);
        if (nomfile == NULL)
        {
            sprintf(msg, "Error: No se pudo reservar memoria para almacenar el nombre del fichero a descargar.\n");
            exit(15);
        }
        fprintf(stderr, "Cliente: %d El fichero a descargar es %s\n", id_cliente, nomfile);

        // El segundo token es el protocolo a usar (T o U)
        token = strtok_r(NULL, "\n", &loc);

        // Inicializamos adecuadamente la estructura CLIENT según el protocolo elegido
        if (strcmp(token, "T") == 0)
        {
            fprintf(stderr, "Cliente: %d La descarga del fichero %s se realizará mediante protocolo TCP\n", id_cliente, nomfile);
            // A RELLENAR
            ...
            ...
        }
        else if (strcmp(token, "U") == 0)
        {
            fprintf(stderr, "Cliente: %d La descarga del fichero %s se realizará mediante protocolo UDP\n", id_cliente, nomfile);
            // A RELLENAR
            ...
            ...
        }
        else // Si no es T ni U se emite un error y se termina
        {
            fprintf(stderr, "Cliente: %d el protocolo para la descarga del fichero %s es desconocido\n", id_cliente, nomfile);
            exit(45);
        }
        // Comprobación de posible error en el clnt_create
        if (cl == NULL)
        {
            clnt_pcreateerror("Error en el clnt_create del hilo Cliente");
            exit(46);
        }
        // bajo la protección del mutex, se realizan todas las invocaciones remotas
        // necesarias para la descarga de este archivo para evitar condiciones de carrera
        // en el acceso a los resultados devueltos via res
        pthread_mutex_lock(&m);
        // Primero comprobar si el fichero existe
		// A RELLENAR
		...
		...
        procesar_etiquetas(id_cliente, cl, nomfile, tam_chunk);
        // Terminadas todas las RPC necesarias para descargar este fichero, se libera
        // el cerrojo para dar oportunidad a otro hilo de entrar
        pthread_mutex_unlock(&m);
        // se puede liberar la estructura CLIENT y el nombre del fichero
        clnt_destroy(cl);
        if (nomfile != NULL)
        {
            free(nomfile);
            nomfile = NULL;
        }
        bzero(buffer, TAMLINEA);
        // Ya que vamos a iniciar otra iteración, tomamos de nuevo el cerrojo
        pthread_mutex_lock(&mpet);
    }
    // Al terminar de leer el fichero de peticiones, se libera el cerrojo
    pthread_mutex_unlock(&mpet);
    return NULL;
}

int main(int argc, char *argv[])
{
    int i;         // Indice para bucles
    pthread_t *th; // array dinámico de referencias a hilos
    datos_hilo *q; // puntero a estructura que se pasará a los hilos
    FILE *fp;      // puntero al fichero de peticiones abierto

    if (argc != 5)
    {
        fprintf(stderr, "Forma de uso: %s <numero_clientes> <ip_serv_srvwww> <fich_descargas> <tam_chunk>\n", argv[0]);
        exit(1);
    }

    // Valida cada argumento y lo asigna a la variable apropiada
    if (!valida_numero(argv[1]))
    {
        fprintf(stderr, "El parametro <numero_clientes> no es un numero.\n");
        exit(2);
    }
    num_clientes = atoi(argv[1]);
    if ((num_clientes < 1) || (num_clientes > 10))
    {
        fprintf(stderr, "El parametro  <numero_clientes> debe ser >= 1 y <= 10\n");
        exit(3);
    }
    ip_srv = strdup(argv[2]);
    if (!valida_ip(argv[2]))
    {
        fprintf(stderr, "La IP introducida no es valida\n");
        exit(4);
    }

    // Intenta abrir el fichero por si hubiera problemas abortar (aunque main
    // no usa este fichero sino que se lo pasará a los hilos Cliente)
    if ((fp = fopen(argv[3], "r")) == NULL)
    {
        fprintf(stderr, "No se puede abrir el fichero con las peticiones WWW a realizar\n");
        exit(5);
    }

    if (!valida_numero(argv[4]))
    {
        fprintf(stderr, "El parametro <tam_chunk> no es un numero.\n");
        exit(6);
    }
    tam_chunk = atoi(argv[4]);
    if (tam_chunk <= 0)
    {
        fprintf(stderr, "El parametro <tam_chunk> debe ser un numero positivo.\n");
        exit(7);
    }
 
    // Ya que los stub de cliente comparten una variable estática, para evitar que los
    // diferentes hilos cliente se pisen entre ellos, se usa un mutex para evitar
    // hacer la misma RPC desde dos hilos a la vez
	// A RELLENAR
	...
	...

    // Hace falta otro mutex para evitar que dos hilos lean a la vez del
    // fichero de peticiones
	// A RELLENAR
	...
	...
    // Reservar espacio para el array dinámico de referencias a hilos
    th = (pthread_t *)malloc(sizeof(pthread_t) * num_clientes);
    if (th == NULL)
    {
        fprintf(stderr, "No se pudo reservar espacio para las referencias a hilos.\n");
        exit(10);
    }

    // Lanzar los hilos
	// A RELLENAR
	...
	...
    // Esperar a que terminen todos los hilos
	// A RELLENAR
	...
	...
    fclose(fp);
    exit(0);
}
