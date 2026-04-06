#include<stdio.h>

int main(int argc, char *argv[]) {
	printf("Al lamar al programa se detectaron %d argumentos: \n", argc-1);

	for (int i = 1; i < argc; i++) {
		printf("Argumento [%d]: %s\n", i, argv[i]);
	}

	return 0;

}
