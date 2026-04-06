#include <netinet/in.h> //Necesario para sockaddr_in
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include "util.h"

typedef struct
{
    char msg[TAMMSG];             // peticion enviada por el cliente
    int s;                        // socket de comunicacion con el cliente
    struct sockaddr_in d_cliente; // dirección IP del cliente
    unsigned char is_stream;      // booleano. es o no orientado a conexion?
} dato_cola;

struct Cola
{
    dato_cola **datos; // la cola en si es un array de punteros a dato_cola
    int head;
    int tail;
    int tam_cola;
    pthread_mutex_t mutex_head;
    pthread_mutex_t mutex_tail;
    sem_t num_huecos;
    sem_t num_ocupados;
};
typedef struct Cola Cola;

void init_cola(Cola *cola, int tam_cola);
void liberar_cola(Cola *cola);
void meter_en_cola(Cola *cola, dato_cola *dato);
dato_cola *sacar_de_cola(Cola *cola);
