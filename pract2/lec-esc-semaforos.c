#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_L  3
#define MAX_E  2

sem_t mutex;
sem_t m_escritor;
int nlectores = 0;
int dato = 0;

void *lector(void *arg) {
    int id = (int)arg;
    while (1) {
        sem_wait(&mutex);
        
        nlectores++;
        if(nlectores == 1){
            sem_wait(&m_escritor);
        }
        sem_post(&mutex);
        usleep(rand() % 1000000);
        // Leer datos
        printf("Lector %d leyendo: %d\n", id, dato);
        usleep(rand() % 1000000); // Retraso aleatorio de hasta 1 segundo (en microsegundos)

        sem_wait(&mutex);
        nlectores--;
        if(nlectores == 0){
            sem_post(&m_escritor);
            usleep(rand() % 1000000);
        }
        sem_post(&mutex);
        usleep(rand() % 1000000);
    }
}

void *escritor(void *arg) {
    int id = (int)arg;
    int aux;
    while (1) {
        //si hay lectores leyendo, hasta que el último no salga, no puede entrar ningún escritor
        aux = dato;
        usleep(rand() % 4000000);
        dato = aux;
        dato++;
        
        printf("Escritor %d escribiendo: %d\n", id, dato);

        usleep(rand() % 2000000); // Retraso aleatorio de hasta 2 segundos (en microsegundos)
    }
}

int main() {

    sem_init(&mutex, 0, 1);
    sem_init(&m_escritor, 0, 1);
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
