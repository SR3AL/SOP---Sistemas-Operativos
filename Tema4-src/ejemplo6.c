// ej6_exec.c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	int status;
	pid_t ret=fork();

	char* argumentos [] = { "ls", "-l", 0 };
     char*  orden [] = { "ls", "-l", 0 };
	
	switch (ret) {
	case -1:
		printf("No se ha podido crear el proceso hijo\n");
		break;
	case 0:
		printf("Soy el hijo con PID %ld y voy a listar el directorio\n", (long)getpid());          
		if (execvp("ls",argumentos)==-1){
			printf("Error en exec\n");
			exit(0);
		}
		break;
	default:
		printf("Soy el padre con PID %ld y mi hijo es %d.\n", (long)getpid(), ret);	
	}
	return 0;

}
