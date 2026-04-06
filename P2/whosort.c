#include <stdio.h>
#include <sys/types.h>

main()
{
    int fds[2]; /* Aquí creamos el array de 2 posiciones que almacenará los extremos del pipe */

    pipe(fds); /*Crea un pipe */

    /* El primer hijo reconecta su entrada estándar (stdin) al flujo
       de salida del pipe y cierra el descriptor de la entrada del pipe */

    pid_t pid;

    if ((pid=fork()) < 0) {
	perror("Falló el fork");
    } 
    else if (pid == 0){
        dup2(fds[0], 0);
        close(fds[1]);
        execlp("sort", "sort", NULL);
    }
    /* El segundo hijo reconecta su salida estándar (stdout) a la
       entrada del pipe y cierra el descriptor de la salida del pipe
    */
    if ((pid=fork()) < 0) {
	perror("Falló el segundo fork");
    }
    else if (pid == 0) {
        dup2(fds[1], 1);
        close(fds[0]);
        execlp("who", "who", NULL);
    }
    /* El padre cerrará ambos descriptores del pipe y esperará
       la finalización de sus hijos
    */
    else {
    	close(fds[0]);
    	close(fds[1]);
    	wait(0);
    	wait(0);
    }
}
