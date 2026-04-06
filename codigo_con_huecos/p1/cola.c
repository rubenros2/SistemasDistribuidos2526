#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cola.h"

void init_cola(Cola *cola, int tam_cola)
{
    register int i;

    if (tam_cola < 1)
    {
        perror("Error: El tamaño de la cola debe ser mayor o igual que 1");
        exit(1);
    }
    if (cola == NULL)
    {
        perror("Error: El puntero a la cola es NULL en inicializar_cola");
        exit(2);
    }
	// A RELLENAR
    cola->tam_cola = tam_cola;
    cola->datos = (dato_cola **)malloc(tam_cola * sizeof(dato_cola *));
    if (cola->datos == NULL)
    {
        perror("Error: No se pudo asignar memoria para los datos de la cola");
        exit(3);
    }
    for (i = 0; i < tam_cola; i++)
    {
        cola->datos[i] = NULL;
    }
    cola->head = 0;
    cola->tail = 0;
    pthread_mutex_init(&cola->mutex_head, NULL);
    pthread_mutex_init(&cola->mutex_tail, NULL);
    sem_init(&cola->num_huecos, 0, tam_cola);
    sem_init(&cola->num_ocupados, 0, 0);
    
}

void liberar_cola(Cola *cola)
{

    // Debe liberarse la memoria apuntada por cada puntero guardado en la cola
    // y la propia memoria donde se guardan esos punteros, así como
    // destruir los semáforos y mutexes
    
	// A RELLENAR
    for (int i = 0; i < cola->tam_cola; i++)
    {
        if (cola->datos[i] != NULL)
        {
            free(cola->datos[i]);
        }
    }
	free(cola->datos);
    pthread_mutex_destroy(&cola->mutex_head);
    pthread_mutex_destroy(&cola->mutex_tail);
    sem_destroy(&cola->num_huecos);
    sem_destroy(&cola->num_ocupados);
}

void meter_en_cola(Cola *cola, dato_cola *dato)
{
    // copiamos el puntero sin mas en la posición adecuada de la cola
    // A RELLENAR
    if (cola == NULL || dato == NULL)
    {
        perror("Error: cola o dato NULL en meter_en_cola");
        exit(10);
    }

    if (sem_wait(&cola->num_huecos) != 0)
    {
        perror("Error: sem_wait(num_huecos) en meter_en_cola");
        exit(11);
    }

    if (pthread_mutex_lock(&cola->mutex_tail) != 0)
    {
        perror("Error: pthread_mutex_lock(mutex_tail) en meter_en_cola");
        exit(12);
    }

    cola->datos[cola->tail] = dato;
    cola->tail = (cola->tail + 1) % cola->tam_cola;

    if (pthread_mutex_unlock(&cola->mutex_tail) != 0)
    {
        perror("Error: pthread_mutex_unlock(mutex_tail) en meter_en_cola");
        exit(13);
    }

    if (sem_post(&cola->num_ocupados) != 0)
    {
        perror("Error: sem_post(num_ocupados) en meter_en_cola");
        exit(14);
    }
}

dato_cola *sacar_de_cola(Cola *cola)
{
    dato_cola *p;

    // A RELLENAR
    if (cola == NULL)
    {
        perror("Error: cola NULL en sacar_de_cola");
        exit(20);
    }

    if (sem_wait(&cola->num_ocupados) != 0)
    {
        perror("Error: sem_wait(num_ocupados) en sacar_de_cola");
        exit(21);
    }

    if (pthread_mutex_lock(&cola->mutex_head) != 0)
    {
        perror("Error: pthread_mutex_lock(mutex_head) en sacar_de_cola");
        exit(22);
    }

    p = cola->datos[cola->head];
    cola->datos[cola->head] = NULL;
    cola->head = (cola->head + 1) % cola->tam_cola;

    if (pthread_mutex_unlock(&cola->mutex_head) != 0)
    {
        perror("Error: pthread_mutex_unlock(mutex_head) en sacar_de_cola");
        exit(23);
    }

    if (sem_post(&cola->num_huecos) != 0)
    {
        perror("Error: sem_post(num_huecos) en sacar_de_cola");
        exit(24);
    }

    return p;
}
