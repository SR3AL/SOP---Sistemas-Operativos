#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define NPROCESOS 4

int main(void)
{
	pid_t ret[NPROCESOS];
	int i, status;

	for (i=0; i<NPROCESOS; i++) {
		ret[i]=fork();
		if (ret[i]==0){
			printf("Soy el hijo %ld con padre %ld\n",(long)getpid(), (long)getppid());
			sleep((i+1)*5);
			exit(0);
		}
	}
	//Ahora a esperar al tercer hijo
	if (waitpid(ret[2],&status,0)==ret[2])
		printf("Mi tercer hijo ya ha terminado\n");
	return 0;
}