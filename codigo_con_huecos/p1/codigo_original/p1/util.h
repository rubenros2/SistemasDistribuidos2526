#ifndef __UTIL_H__

#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <string.h>

#define FALSO 0
#define CIERTO 1
#define TAMMSG 256
#define TAMLINEA 512
#define TAMMAXRESPUESTA 256
#define TAMNOMFILE 128
#define TAMDIR 256
#define TAMCMD 20

int max(int a, int b);
int valida_numero(char *str);
int valida_ip(char *ip);
double randRange(double min, double max);
void log_debug(char *msg);

#define __UTIL_H__
#endif
