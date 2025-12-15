#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_L  3
#define MAX_E  2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_lectores = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_escritores = PTHREAD_COND_INITIALIZER;

int nlectores = 0;
int dato = 0;
void *lector(void *arg) {
    int id = (int)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        if(nlectores == -1){
            pthread_cond_wait(&cond_lectores, &mutex);
        }
        nlectores++;
        // Leer datos
        printf("Lector %d leyendo: %d\n", id, dato);
        usleep(rand() % 1000000); // Retraso aleatorio de hasta 1 segundo (en microsegundos)

        nlectores--;
        if(nlectores == 0){
            pthread_cond_signal(&cond_lectores);
        }
        pthread_mutex_unlock(&mutex);
        usleep(rand() % 1000000);
    }
}

void *escritor(void *arg) {
    int id = (int)arg;
    int aux;
    while (1) {
        pthread_mutex_lock(&mutex);
        if(nlectores > 0){
            pthread_cond_wait(&cond_escritores, &mutex);
            nlectores=-1;
        }        
        //si hay lectores leyendo, hasta que el último no salga, no puede entrar ningún escritor
        aux = dato;
        usleep(rand() % 2000000);
        dato = aux;
        dato++;
        
        printf("Escritor %d escribiendo: %d\n", id, dato);

        usleep(rand() % 2000000); // Retraso aleatorio de hasta 2 segundos (en microsegundos)
        nlectores = 0;
            pthread_cond_broadcast(&cond_lectores);
            pthread_cond_broadcast(&cond_escritores);
        
        usleep(rand() % 2000000);
        pthread_mutex_unlock(&mutex);
        usleep(rand() % 2000000);
    }
}

int main() {

    // MAX_L lectores y MAX_E escritores
    pthread_t lectores[MAX_L], escritores[MAX_E]; 
    int i;
    pthread_attr_t atrib;

    // Inicializar la semilla del generador de números aleatorios
    srand(time(NULL)); 

    // Inicializar la estructura pthread_attr
    pthread_attr_init(&atrib);

    // Crear lectores
    for(i = 0; i < MAX_L; i++){
        pthread_create(&lectores[i], &atrib, lector, (void*)i);
    }

    // Crear escritores
    for(i = 0; i < MAX_E; i++){
        pthread_create(&escritores[i], &atrib, escritor, (void*)i);
    }

    // Esperar a que los hilos terminen
    for(i = 0; i < MAX_L; i++){
        pthread_join(lectores[i], NULL);
    }

    for(i = 0; i < MAX_E; i++){
        pthread_join(escritores[i], NULL);
    }
    printf("Acaba el main\n");
    return 0;
}
