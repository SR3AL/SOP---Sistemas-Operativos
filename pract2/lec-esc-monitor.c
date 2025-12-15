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
int nesc_esperando = 0;
int nesc_activos = 0;

void comenzar_lectura(){
    pthread_mutex_lock(&mutex);
        if(nlectores == -1){
            pthread_cond_wait(&cond_lectores, &mutex);
        }
    nlectores++;
    pthread_mutex_unlock(&mutex);
}

void terminar_lectura(){
    pthread_mutex_lock(&mutex);
    nlectores--;
    if(nlectores == 0){
        pthread_cond_signal(&cond_escritores);
    }
    pthread_mutex_unlock(&mutex);
}

void *lector(void *arg) {
    int id = (int)arg;
    while (1) {
        comenzar_lectura();
        printf("nlectores: %d | nesc_esperando: %d | Lector %d leyendo: %d \n", nlectores, nesc_esperando, id, dato);
        usleep(rand() % 1000000);
        terminar_lectura();
        usleep(rand() % 2000000);
    }
}

void comenzar_escritura(){
    nesc_esperando++;
    pthread_mutex_lock(&mutex);
    if(nlectores > 0){
        pthread_cond_wait(&cond_escritores, &mutex);
    }
    nesc_esperando--;
    nesc_activos++;
    nlectores = -1;
    //si hay lectores leyendo, hasta que el último no salga, no puede entrar ningún escritor
    pthread_mutex_unlock(&mutex);
}

void terminar_escritura(){
    pthread_mutex_lock(&mutex);
    nesc_activos--;
    nlectores = 0; // Retraso aleatorio de hasta 2 segundos (en microsegundos)        nlectores = 0;
    pthread_cond_broadcast(&cond_lectores);
    pthread_cond_broadcast(&cond_escritores);
    pthread_mutex_unlock(&mutex);
}

void *escritor(void *arg) {
    int id = (int)arg;
    int aux;
    while (1) {
        comenzar_escritura();
        aux = dato;
        usleep(rand() % 2000000);
        aux++;
        usleep(rand() % 2000000);
        dato = aux;
        printf(" --> nesc_activos: %d | Escritor %d escribiendo: %d \n", nesc_activos, id, dato);
        terminar_escritura();
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
