#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "srvwww.h"
#include "util.h"

// Función de utilidad que determina si los caracteres de una cadena
// son todos numericos
int valida_numero(char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
        { // si el caracter no es un numero retorna falso
            return FALSO;
        }
        str++; // apuntamos al siguiente caracter
    }
    return CIERTO;
}

// Función de utilidad que valida si una cadena de caracteres representa
// una IPv4 valida
int valida_ip(char *ip)
{ // comprueba si la IP es valida o no
    int num, dots = 0;
    char *mip;
    char *ptr;

    if (ip == NULL)
        return FALSO;
    mip = strdup(ip);       // para no modificar la ip original
    ptr = strtok(mip, "."); // extrae los tokens de la cadena delimitados por '.'
    if (ptr == NULL)
        return FALSO;
    while (ptr)
    {
        if (!valida_numero(ptr)) // comprueba si la subcadena es un numero o no
            return FALSO;
        num = atoi(ptr); // convierte la subcadena a entero
        if (num >= 0 && num <= 255)
        {
            ptr = strtok(NULL, "."); // extrae la siguiente subcadena
            if (ptr != NULL)
                dots++; // incrementa el contador de delimitadores
        }
        else
            return FALSO;
    }
    if (dots != 3) // si el numero de '.' es distinto de 3, retorna falso
        return FALSO;
    return CIERTO;
}

// Función de utilidad para depuración. Emite por pantalla el mensaje
// que se le pasa como parámetro, pero pone delante del mensaje un
// timestamp, para poder ordenar la salida por si saliera desordenada
//
// Ejemplo de uso:
//
//  log_debug("Mensaje a imprimir")
//
// Más ejemplos en el programa principal.
void log_debug(char *msg)
{
    struct timespec t;
    clock_gettime(_POSIX_MONOTONIC_CLOCK, &t);
    printf("[%ld.%09ld] %s", t.tv_sec, t.tv_nsec, msg);
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

// Función que devuelve cierto si el nombre de una carpeta que se le pasa
// como argumento existe y es un directorio
unsigned char existe_carpeta(const char *nombre)
{
    struct stat buffer;
    return (stat(nombre, &buffer) == 0) && S_ISDIR(buffer.st_mode);
}
