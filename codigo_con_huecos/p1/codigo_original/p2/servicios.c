#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "srvwww.h"
#include "util.h"

// Esta variable externa es asignada en main, a través del código
// inyectado por el script en python.
// Su valor es usado por los diferentes servicios implementados aqui
extern char *documentroot;

void libera_lista(listatrozos *l)
{
    // Función que libera la memoria de cada nodo de la lista
    // enlazada de trozos, que se usa en el servicio getchunk_1_svc
    // Será llamada desde ese servicio para liberar la memoria
    // reservada durante la ejecución previa del mismo servicio

    listatrozos *aux;
    while (l != NULL)
    {
        aux = l;
        l = l->sgte;
        free(aux->trozo.chunk_val); // Liberar memoria del trozo
        free(aux);
    }
}

// función que devuelve el tamaño de un fichero en bytes
long get_file_size(const char *filename)
{
    struct stat st;

    st.st_size = -1; // No puede haber un fichero con tamaño negativo
    if (stat(filename, &st) == -1)
    {
        fprintf(stderr, "Error al obtener el tamaño del fichero %s: %s\n", filename, strerror(errno));
        return -1;
    }
    return st.st_size;
}

Resultado *existfile_1_svc(cadenaS *fname, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    static Resultado res;
    struct stat buf; // Para la llamada a stat()

    // Comprueba si el archivo documentroot/fname existe
    // y retorna apropiadamente el resultado en res
	// A RELLENAR
	...
	...

    return (&res);
}

Resultado *getsize_1_svc(cadenaS *fname, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    static Resultado res;
    long tam;

    // Abrir documentroot/fname y contar su número de líneas
    // retornando apropiadamente el resultado en res
	// A RELLENAR
	...
	...
}

void error_y_cierra_fichero(char *msg, int fd, Resultado *res)
{
    // Función auxiliar de utilidad que prepara la estructura Resultado
    // con un mensaje de error, y cierra el fichero fd
    // A RELLENAR
    ...
    ...
}

Resultado *getchunk_1_svc(param1 *fileitem, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    int fd;               // Fichero del que vamos a leer el trozo
    char trozo[LENTROZO];  // Buffer para almacenar el trozo leído
    static Resultado res;
    long offset; // Offset desde el que se va a leer el fichero
    int pendientes; // Variable para almacenar el número de bytes pendientes de leer
    int tamtrozo; // Variable para almacenar el tamaño del trozo leído
    int leidos; // Variable para almacenar el número de bytes leídos en cada iteración
    int acumulados = 0; // Variable para acumular el número de bytes leídos

    // Variables auxiliares para ir construyendo la lista enlazada
    static listatrozos *primera = NULL; // puntero al primer nodo de la lista enlazada
    listatrozos *ultima = NULL;  // puntero al último nodo de la lista enlazada
    listatrozos *aux=NULL; // Puntero auxiliar 

    // Liberar memoria de la lista de trozos que pudiera haber sido
    // reservada en una ejecución previa de este mismo servicio
    libera_lista(primera);
    primera=NULL;

    // Abrir documentroot/fname
    sprintf(filename, "%s/%s", documentroot, fileitem->filename);
    fprintf(stderr, "Servidor: El cliente pide un trozo del fichero fichero %s a partir del offset %ld de tamaño %d bytes\n", filename, fileitem->offset, fileitem->size);
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        error_y_cierra_fichero("Error al abrir el archivo para obtener un trozo\n", fd, &res);
        return (&res);
    }

    // Mover el puntero del fichero al offset indicado
    offset = fileitem->offset;
    pendientes = fileitem->size; // Número de bytes que se van a leer
    
    // Comprobaciones básicas, el offset y pendientes no pueden ser negativos
    // A RELLENAR
    ...
    ...

    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        error_y_cierra_fichero("Error al mover el puntero del fichero al offset indicado\n", fd, &res);
        return (&res);
    }
    fprintf(stderr, "Servidor: Posicionados en el fichero %s a partir del offset %ld \n", filename, fileitem->offset);
    
    // Leer el fichero en bloques de LENTROZO bytes como mucho
    // y añadirlos a la lista enlazada de trozos
    // Si se alcanza el final del fichero, se sale del bucle
    tamtrozo = min(LENTROZO, pendientes); // Tamaño del trozo a leer
    fprintf(stderr, "Servidor: Tamaño del trozo a leer del fichero %s a partir del offset %ld es %d\n", filename, fileitem->offset, tamtrozo);
    acumulados = 0; // Reiniciar el contador de bytes leídos
    explicit_bzero(trozo, tamtrozo); // Limpiar el buffer del trozo
    
    while ((tamtrozo > 0) && (leidos=read(fd, trozo, tamtrozo)) > 0)
    {
        if (leidos < 0)
        { // Si hay error al leer
            error_y_cierra_fichero("Error al leer el fichero\n", fd, &res);
            if (primera!=NULL) libera_lista(primera); // Liberar memoria de la lista
            primera = NULL; // Reiniciar la lista
            return (&res);
        }
        fprintf(stderr, "Servidor: Leidos %d bytes del fichero %s \n", leidos, filename);
        if (leidos < tamtrozo)
        { // Si se leen menos bytes de los que se deben, se ajusta el tamaño del trozo
            tamtrozo = leidos; // Ajustar el tamaño del trozo leído
        }
        acumulados += leidos; // Acumular el número de bytes leídos
        pendientes -= leidos; // Actualizar el número de bytes pendientes de leer
        // Caso especial, si es el primer trozo se crea el primer nodo de la lista
        if (primera == NULL)
        {
            primera = (listatrozos *)malloc(sizeof(listatrozos));
            if (primera == NULL)
            {
                error_y_cierra_fichero("Error al reservar memoria para el primer nodo de la lista\n", fd, &res);
                return (&res);
            }
            // Reservar memoria para el trozo
            // A RELLENAR
            ...
            ...

            if (primera->trozo.chunk_val == NULL)
            {
                error_y_cierra_fichero("Error al reservar memoria para el trozo\n", fd, &res);
                return (&res);
            }
            
            memmove(primera->trozo.chunk_val, trozo, tamtrozo); // Copiar el trozo leído al nodo
            primera->trozo.chunk_len = tamtrozo; // Guardar el tamaño del trozo
            primera->sgte = NULL;
            ultima = primera;
        }
        else // El resto de trozos se añade al final de la lista
        {
            aux = (listatrozos *)malloc(sizeof(listatrozos));
            if (aux == NULL)
            {
                error_y_cierra_fichero("Error al reservar memoria para un nodo de la lista\n", fd, &res);
                libera_lista(primera); // Liberar memoria de la lista
                primera = NULL; // Reiniciar la lista
                return (&res);
            }
            // Reservar memoria para el trozo
            // A RELLENAR
            ... 
            ...

            if (aux->trozo.chunk_val == NULL)
            {
                error_y_cierra_fichero("Error al reservar memoria para el trozo\n", fd, &res);
                libera_lista(primera); // Liberar memoria de la lista
                primera = NULL; // Reiniciar la lista
                return (&res);
            }
            memcpy(aux->trozo.chunk_val, trozo, tamtrozo); // Copiar el trozo leído al nodo
            aux->trozo.chunk_len = tamtrozo; // Guardar el tamaño del trozo
            aux->sgte = NULL;
            if (ultima) ultima->sgte = aux; // Enlazar el nodo anterior con el nuevo 
            ultima = aux; // Actualizar el puntero al último nodo   
        }
        tamtrozo = min(LENTROZO, pendientes); // Actualizar el tamaño del trozo a leer
        bzero(trozo, tamtrozo); // Limpiar el buffer del trozo        

    }
    
    if (acumulados != fileitem->size)
    {
        fprintf(stderr, "Servidor: En el fichero %s a partir del offset %ld  había que leer %d bytes y leimos %d\n", filename, fileitem->offset, fileitem->size, acumulados);
        // Si no se han leído todos los bytes que se debían, se informa
        error_y_cierra_fichero("Error: No se han leído todos los bytes que se debían\n", fd, &res);
        libera_lista(primera); // Liberar memoria de la lista
        primera = NULL; // Reiniciar la lista
        return (&res);
    }

    close(fd);
    // A RELLENAR
    ...
    ...
    return (&res);
}
