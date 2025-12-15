/*-----------------------------------------------------+
|     E J E C U C I O N . C                           |
+-----------------------------------------------------+
|     Asignatura :  SOP-GIIROB                        |
|     Descripcion:                                    |
+-----------------------------------------------------*/
#include "defines.h"
#include "redireccion.h"
#include "ejecucion.h"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "profe.h"

int ejecutar(int nordenes, int *nargs, char **ordenes, char ***args, int bgnd, char **ant)
{
    int i, aux, n;
    char buffer[255];
    // Ampliación cd
    if (args[0][1] != NULL && nargs[0] > 1 && strcmp(ordenes[0], "cd") == 0)
    { // Aseguro que haya más de cd(es decir, que tenga una ruta); y que no esté vacia
        const char *check = args[0][1];
        if (strcmp(check, "-") == 0)
        { // Compruebo si es el caso del 'cd -'
            if (ant[0] == "")
            {
                printf("No hay ruta previa.\n");
                return OK;
            }
            else
            {
                char temp[255];
                strcpy(temp, ant[1]); // Guardamos la dirección actual
                if (chdir(ant[0]) == 0)
                {
                    printf("Has vuelto al directorio anterior: %s\n", ant[0]);
                    strcpy(ant[1], ant[0]);
                    strcpy(ant[0], temp);
                    return OK;
                }
                else
                {
                    perror("Error al volver al directorio anterior.");
                    return ERROR;
                }
            }
        }
        else
        {
            if (strlen(ant[1]) == 0)
            {
                // Guardamos el directorio actual en ant[1] si es el primer uso
                if (getcwd(ant[1], 256) == NULL)
                {
                    perror("getcwd");
                    return ERROR;
                }
                strcpy(ant[0], ant[1]);
            }
            // 'cd' con argumentos (uso normal)
            if (chdir(args[0][1]) == 0)
            {
                strcpy(ant[0], ant[1]);         // Copio en el que estoy en el anterior
                getcwd(ant[1], sizeof(buffer)); // Vemos el tamaño que ocupa del buffer
                if (ant[1] != NULL)
                {
                    printf("Directorio cambiado a %s\n", ant[1]);
                }
                else
                {
                    perror("No puedes dirigirte a una dirección NULL: ");
                    return ERROR;
                }
            }
            else
            {
                perror("Error al cambiar de directorio.");
                return ERROR;
            }
        }
    }
    else if (strcmp(ordenes[0], "cd") == 0)
    { // 'cd' sin argumentos redirecciono a home
        char *nuevo_directorio = args[i][1];
        nuevo_directorio = getenv("HOME");
        if (strcmp(ant[1], "") == 0)
        {
            strcpy(ant[1], nuevo_directorio); // Aseguro que funcione colocando '/' al final de la ruta
            if (strcmp(ant[0], "") == 0)
                strcpy(ant[0], ant[1]);
        }
        if (chdir(nuevo_directorio) == 0)
        {
            strcpy(ant[0], ant[1]);
            if (ant[1] != NULL)
            {
                getcwd(ant[1], sizeof(buffer));
                printf("Directorio cambiado a %s\n", ant[1]);
                return OK;
            }
            else
            {
                printf("Directorio cambiado a %s\n", nuevo_directorio);
                return OK;
            }
        }
        else
        {
            perror("Error al cambiar de directorio.");
            return ERROR;
        }
    }

    // Ampliación exit()
    if (strcmp(ordenes[0], "exit") == 0 || strcmp(ordenes[0], "exit()") == 0)
    { // exit
        printf("Saliendo del programa con exit...\n");
        exit(0);
    }

    if (sscanf(ordenes[0], "exit(%d)", &n) == 1)
    { // exit(n)
        printf("Saliendo del programa con exit(%d)...\n", n);
        exit(n);
    }
    for (int i = 0; i < nordenes; i++)
    {
        aux = fork();
        if (aux == 0)
        {
            redirigir_entrada(i);
            redirigir_salida(i);
            cerrar_fd();
            execvp(ordenes[i], args[i]);
        }
    }
    cerrar_fd();
    for (int i = 0; i < nordenes; ++i)
    {
        wait(NULL);
    }
    return OK;
} // Fin de la funcion "ejecutar"
