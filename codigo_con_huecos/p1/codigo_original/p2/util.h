#ifndef __UTIL_H__

#define CIERTO 1
#define FALSO 0
#define TAMLINEA 256
#define TAMMSG 128
#define TAMDIR 256

int valida_numero(char *str);
int valida_ip(char *ip);
void log_debug(char *msg);
int min(int a, int b);
unsigned char existe_carpeta(const char *nombre);

#define __UTIL_H__
#endif
