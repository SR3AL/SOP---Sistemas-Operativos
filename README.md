# Sistemas Operativos (SOP)

Repositorio del curso de Sistemas Operativos (SOP) de la Universidad Politécnica de Valencia(UPV).
Aquí encontrarás un breve resumen de las prácticas realizadas durante el curso 24/25.

## Objetivos y planificación

El objetivo de esta práctica es adquirir soltura en la programación concurrente con hilos **POSIX**. Para llevar a cabo dicho objetivo será necesario trabajar con llamadas al sistema y en concreto con las responsables de **gestión de creación y sincronización de hilos de ejecución** (_pthreads_). Todas estas llamadas se estudian en las clases de teoría de la asignatura.

Esta práctica se compone de **4 sesiones** en las que se irán abordando la
creación y sincronización de hilos utilizando diferentes estrategias.

| Semana   | Contenido                                                   |
| -------- | ----------------------------------------------------------- |
| Semana 1 | Programación con hilos. Problema de sección crítica         |
| Semana 2 | Sección crítica: solución con semáforos POSIX               |
| Semana 3 | Sección crítica: solución con mutexes y variables condición |
| Semana 4 | Sección crítica: solución con monitores (ampliación)        |

## Introducción

Esta primera parte consiste en introducir una prioridad para los escritores, lo que significa que, si hay un **escritor** esperando para escribir, <ins>no se permitirá la entrada a nuevos lectores hasta que el escritor haya terminado</ins>.

Las <ins>reglas del problema</ins> son las siguientes:

- **Lectores:**
  1. Pueden leer simultáneamente.
  2. Si hay un escritor esperando, no pueden entrar nuevos lectores.
     acceder libremente.
- **Escritores:**
  1. Solo un escritor puede escribir a la vez.
  2. Si hay lectores leyendo, los escritores deben esperar hasta que todos los lectores
     hayan terminado.
  3. Si hay otro escritor escribiendo, un nuevo escritor debe esperar hasta que el
     escritor actual haya terminado.

## Práctica 2: Programación Concurrente

<details open>
    <summary>
        <span style="text-decoration: underline; font-size: 21px">
        Semana 1: Programación con hilos. Problema de sección crítica
        </span>
    </summary>
<br>
Como objetivo de esta primera sesión, se pretende completar el archivo `lec-esc-hilos.c` con las llamadas al sistema que permitan lanzar los hilos, sincronizar su finalización y comprobar que durante su ejecución se produce una condición de carrera.

> [!NOTE]
> El archivo se encuentra en `pract2/lec-esc-hilos.c`.

Las <ins>acciones</ins> que se deben realizar en esta práctica son:

- Completar el código de la función **main** para crear los hilos “lectores” y los hilos “escritores”, así como el código que permite esperar a todo ellos una vez creados (ver los comentarios en el código)
- Compilar el código con el comando: `gcc -o le-hilos lec-esc-hilos.c -lpthread`.

### _Comentarios sobre el código ya implementado:_

Como se puede apreciar en el archivo `lec-esc-hilos.c`, hay dos funciones.

1. Función `void *lector(void *arg)` : encargada de imprimir cuando un lector está leyendo(y diciendo qué lee en cada momento). Esta función, va acompañada de un `usleep(rand() % 1000000);` que invoca un retardo aleatorio de hasta 1 segundo.

2. Función `void *escritor(void *arg)` : encargada de varias cosas. En primer lugar, recoge el **id**(dato recogido en el argumento `arg`) del escritor que va a escribir. A continuación, antes de imprimir qué escritor está escribiendo y el qué, se invoca una variable llamada `aux` que será la encargada de recoger la variable global `dato`. Ésta, incrementará su valor en 1 y luego ese dato será asignado nuevamente `dato` mediante `dato = aux;`. <br>
   Al igual que en la función del **lector**, se invoca un retardo aleatorio de hasta 2 segundo mediante `usleep(rand() % 2000000);` entre cada uno de los procesos y se imprime en cada momento el número de escritor y el contenido que está escribiendo.

El contenido de la función `main()` es usado para invocar a los lectores con la función `pthread_t lectores[MAX_LECTORES];` y a los escritores con `pthread_t escritores[MAX_ESCRITORES];`. Además, se inician las estructuras pthread(`pthread_attr_init(&atrib);`) y se hacen dos bucles con el fin de crear los respectivos lectores y escritores gracias a la función `pthread_create()`. Por último, se hace uso de la función pthread_join() para esperar a que todos los hilos terminen su ejecución y así imprimir el mensaje `Acaba el main\n`.

</details>

<details>
    <summary>
        <span style="text-decoration: underline; font-size: 21px">
        Semana 2: Sección crítica: solución con semáforos POSIX
        </span>
    </summary>
<br>

En esta segunda parte de la práctica, se pretende completar el archivo `lec-esc-semaforos.c` con las llamadas al sistema que permitan solucionar la condición de carrera que previamente hemos provocado, en el código de los lectores y escritores, con ayuda de los semáforos **POSIX**.

> [!NOTE]
> El archivo se encuentra en `pract2/lec-esc-semaforos.c`.

Se deberán de realizar los siguientes **pasos**:

1. Copia el fichero `lec-esc-hilos.c` de la sesión anterior en un fichero llamado
   `lec-esc-semaforos.c`.
2. Para poder seguir las reglas del problema de los **lectores-escritores** se recomienda
   definir _dos semáforos_: uno llamado `mutex` para el acceso a la variable compartida y
   otro, `m_escritores`, para gestionar la **exclusión mutua entre escritores**.
3. Hay que definir una variable `nlectores` que lleve la cuenta de los lectores que hay
   en leyendo. De manera que, si hay lectores leyendo, hasta que el último no salga,
   no puede entrar ningún escritor y mientras haya un lector leyendo, los lectores
   pueden seguir entrando a leer.
4. Una vez definidos los semáforos, introduce las instrucciones `sem_wait` (operación
   P) y `sem_post` (operación V) en los lugares que corresponda para que el código
   funcione y lo haga libre de condiciones de carrera.
5. Compila el código y comprueba que se ha eliminado la condición de carrera con el comando: `gcc -o lec-semaforos lec-esc-semaforos.c -lpthread`

### _Comentarios sobre el código ya implementado:_

Para hacer uso de los semáforos en el código, es necesario incluir la librería `#include <semaphore.h>`. A continuación, se definen los semáforos `sem_t mutex;` y `sem_t m_escritores;` y la variable `int nlectores = 0;` para llevar la cuenta de los lectores que hay leyendo. <br>

Antes de crear los hilos, es necesario inicializar los semáforos con `sem_init(&mutex, 0, 1);` y `sem_init(&m_escritores, 0, 1);`, donde el tercer parámetro indica el valor inicial del semáforo.<br>

En la función del **lector**, se debe proteger el acceso a la variable `nlectores` con el semáforo `mutex`. Por lo tanto, antes de incrementar `nlectores`, se debe hacer un `sem_wait(&mutex);` y después de incrementar `nlectores`, se debe hacer un `sem_post(&mutex);`. Además, si `nlectores` es igual a 1, significa que es el primer lector que entra, por lo que debe hacer un `sem_wait(&m_escritores);` para bloquear a los escritores. Al salir, si `nlectores` es igual a 0, significa que es el último lector que sale, por lo que debe hacer un `sem_post(&m_escritores);` para permitir la entrada de escritores.
En la función del **escritor**, se debe debe actualizar la variable `aux` igualandola a `dato`. Después, haremos un `usleep(rand() % 2000000);` para simular el tiempo de escritura y finalmente se asigna `dato = aux;` y a dato se le suma 1. Y finalmente se imprimirá el mensaje indicando qué escritor ha escrito y qué dato ha escrito, acompañado de otro usleep() para simular el tiempo entre escrituras.

</details>

<details>
    <summary>
        <span style="text-decoration: underline; font-size: 21px">
        Semana 3: Sección crítica: solución con mutexes y variables condición
        </span>
    </summary>
<br>

Esta tercera parte de la práctica, consiste en obtener un código libre de condiciones de carrera utilizando mutexes y las variables condición.
<br>
_Debemos partir del código de la primera sesión en el que se producía la condición de carrera._
<br>
Siguiendo los siguientes pasos:

1. Copia el fichero `lec-esc-hilos.c` de la sesión anterior en un fichero llamado
   `lec-esc-hilos-condicion.c`
2. Para poder seguir las reglas del problema de los **lectores-escritores** se recomienda
   definir un mutex llamado `mutex` y un par de variables `cond_lectores` y
   `cond_escritores`, para gestionar la exclusión mutua entre escritores, y entre
   lectores y escritores.
3. Hay que definir una variable `nlectores` que lleve la cuenta de los lectores que hay
   en leyendo. De manera que, si hay lectores leyendo, hasta que el último no salga,
   no puede entrar ningún escritor y mientras haya un lector leyendo, los lectores
   pueden seguir entrando a leer.
4. Una vez definidos los **mutexes** y las **variables condición**, introduce las instrucciones
   `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_cond_wait`,
   `pthread_cond_signal` y `pthread_cond_broadcast` en los lugares que
   corresponda para que el código funcione y lo haga libre de condiciones de carrera.
5. Compila el código y comprueba que se ha eliminado la condición de carrera con el comando: `gcc -o le-condicion lec-esc-hilos-condicion.c -lpthread`

> [!NOTE]
> El archivo se encuentra en `pract2/lec-esc-semaforos.c`.

### _Comentarios sobre el código ya implementado:_

Para hacer uso de los mutexes y las variables condición en el código, es necesario incluir la librería `#include <pthread.h>`.
<br>

A continuación, se definen el mutex `pthread_mutex_t mutex;` y las variables condición `pthread_cond_t cond_lectores;` y `pthread_cond_t cond_escritores;`, así como la variable `int nlectores = 0;` para llevar la cuenta de los lectores que hay leyendo. <br>

En la función del **lector**, se debe proteger el acceso a la variable `nlectores` con el mutex `mutex`. Por lo tanto, antes de incrementar `nlectores`, se debe hacer un `pthread_mutex_lock(&mutex);` y después de incrementar `nlectores`, se debe hacer un `pthread_mutex_unlock(&mutex);`. Además, si `nlectores` es igual a 1, significa que es el primer lector que entra, por lo que debe hacer un `pthread_cond_wait(&cond_escritores, &mutex);` para bloquear a los escritores. Al salir, si `nlectores` es igual a 0, significa que es el último lector que sale, por lo que debe hacer un `pthread_cond_signal(&cond_escritores);` para permitir la entrada de escritores.

> [!IMPORTANT]
> Ten cuidado, acuerdate de SIEMPRE cerrar el mutex con un `pthread_mutex_unlock(&mutex);` después de cada `pthread_cond_wait(&cond_escritores, &mutex);`, ya que al llamar a `pthread_cond_wait`, el mutex se desbloquea automáticamente mientras espera y se vuelve a bloquear cuando se despierta. Si no cierras el mutex después de la espera, podrías provocar un bloqueo en el programa.

En la función del **escritor**, se debe hacer un `pthread_mutex_lock(&mutex
);` antes de escribir y un `pthread_mutex_unlock(&mutex);` después de escribir. Además, si hay lectores leyendo, el escritor debe hacer un `pthread_cond_wait(&cond_lectores, &mutex);` para esperar hasta que todos los lectores hayan terminado. Una vez que el escritor ha terminado de escribir, debe hacer un `pthread_cond_broadcast(&cond_lectores);` para permitir la entrada de lectores.

</details>

<details>
    <summary>
        <span style="text-decoration: underline; font-size: 21px">
        Semana 4: Sección crítica: solución con monitores (ampliación)
        </span>
    </summary>
<br>

Esta cuarta sesión de la práctica, consiste en obtener un código libre de cdiciones de carrera utilizando monitores.
<br>

_Debemos partir del código de la primera sesión en el que se producía la condición de carrera._
<br>

Siguiendo los siguientes **pasos**:

1. Copia el fichero `lec-esc-hilos.c` de la sesión anterior en un fichero llamado `lec-esc-monitor.c`.
2. Para resolver el problema se deben definir las siguientes funciones del monitor:
   - `comenzar_lectura`
   - `terminar_lectura`
   - `comenzar_escritura`
   - `terminar_escritura`
3. Hay que definir las variables `nlectores`, `nesc_esperando`, `nesc_activos`.
   que lleve la cuenta de todos ellos.
4. Una vez definidas las funciones del monitor reescribe las funciones de los hilos de la siguiente manera.

```cpp
void *lector(void *arg) {
    for (int i = 0; i < 5; i++) {
    comenzar_lectura();
    printf("Lector leyendo...\n");
    terminar_lectura();
    }
    pthread_exit(NULL);
}

void *escritor(void *arg) {
    for (int i = 0; i < 5; i++) {
    comenzar_escritura();
    printf("Escritor escribiendo...\n");
    terminar_escritura();
    }
    pthread_exit(NULL);
}
```
5. Por último, compila el código y comprueba que se ha eliminado la condición de carrera con el comando: `gcc -o lec-monitor lec-esc-monitor.c -lpthread`
</details>


## Autor Info
Sergio Real Gonzalvo
<br>
Email: sergiete.real@gmail.com
<br>
Github: https://github.com/SR3AL