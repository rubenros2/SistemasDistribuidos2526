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
	...
	...
}

void liberar_cola(Cola *cola)
{

    // Debe liberarse la memoria apuntada por cada puntero guardado en la cola
    // y la propia memoria donde se guardan esos punteros, así como
    // destruir los semáforos y mutexes
    
	// A RELLENAR
	...
	...
}

void meter_en_cola(Cola *cola, dato_cola *dato)
{
    // copiamos el puntero sin mas en la posición adecuada de la cola
    // A RELLENAR
    ...
    ...
}

dato_cola *sacar_de_cola(Cola *cola)
{
    dato_cola *p;

    // A RELLENAR
    ...
    ...
}
