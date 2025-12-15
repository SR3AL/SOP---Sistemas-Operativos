#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main (void)
{ 
     pid_t valor;
     int i = 0;
     printf("PID antes del fork(): %ld\n", (long) getpid()); 
     valor=fork();
     if (valor > 0) {
       printf("Padre PID: %ld\n", (long) getpid());
       i++;
     } else {
       printf("Hijo PID: %ld\n", (long) getpid());
     }
     printf("Proceso [%ld]-> i = %d\n", (long) getpid(), i);
     return 0;
}