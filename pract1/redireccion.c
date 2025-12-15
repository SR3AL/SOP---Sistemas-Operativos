/*-----------------------------------------------------+
 |     R E D I R E C C I O N . C                       |
 +-----------------------------------------------------+
 |     Version    :
 |     Autor      :  SERGIO REAL GONZALVO
 |     Asignatura :  SOP-GIIROB
 |     Descripcion:
 +-----------------------------------------------------*/
#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include <unistd.h>

REDIRECCION_ORDENES red_ordenes;

void redireccion_ini(void)
{
    for (int i = 0; i < PIPELINE; i++)
    {
        red_ordenes[i].entrada = -1;
        red_ordenes[i].salida = -1;
    }
} // Inicializar los valores de la estructura cmdfd

int pipeline(int nordenes, char *infile, char *outfile, int append, int bgnd)
{
    
    /*
    Capturamos las excepciones
    */
    if (nordenes < 1)
        return ERROR;
    if (nordenes > PIPELINE)
        return ERROR;

    /*
    Inicializamos la estructura de redirección
     */
    redireccion_ini();

    /*
    Implementar el código que se encargue de preparar la redirección de la entrada y
    salida estándar, si fuera necesario. Para ello, se almacenará la información sobre los
    descriptores de entrada y/o salida redirigidos, en el vector red_ordenes.
    */

    // Redirigimiento de la entrada
    if (strcmp(infile, "") != 0)
    {
        int fd = open(infile, O_RDONLY);
        if (fd < 0)
        {
            perror("ERROR AL ABRIR EL ARCHIVO DE ENTRADA");
            return ERROR;
        }
        red_ordenes[0].entrada = fd;
    }
    else
    {
        red_ordenes[0].entrada = STDIN_FILENO;
    }

    /*
    En caso de ejecución en segundo plano se redirigirá la entrada estándar de la
    primera orden al dispositivo /dev/null.
    */

    if (bgnd)
    {
        int dev_null_fds = open("/dev/null", O_RDONLY);
        if (dev_null_fds < 0)
        {
            perror("Error al abrir /dev/null");
            return ERROR;
        }
        red_ordenes[0].entrada = dev_null_fds;
    }

    // Redirigimiento de la salida
    if (strcmp(outfile, "") != 0)
    {
        int fd;
        // Creamos las dos flags necesarias
        int flags = O_WRONLY | O_CREAT;
        // Comprobamos si se tendrá que hacer un append o trunc
        if (append == 1) // A parte de las flags anteriores añadimos trunk o append
        {
            flags |= O_APPEND;
        }
        else
        {
            flags |= O_TRUNC;
        }
        // Abrimos el archivo de salida otorgando los permisos necesarios para poder crear el archivo en caso de que no exista
        fd = open(outfile, flags, 0666);
        if (fd < 0)
        {
            perror("ERROR AL ABRIR EL ARCHIVO DE SALIDA");
            return ERROR;
        }
        red_ordenes[nordenes - 1].salida = fd;
    }
    else
    {
        red_ordenes[nordenes - 1].salida = STDOUT_FILENO;
    }

    /*
    Crear tantos tubos como sean necesarios mediante la llamada pipe, para conectar
    las órdenes recibidas y guardar los descriptores devueltos por cada llamada en el
    vector red_ordenes.
    */

    int fds[2];
    for (int i = 0; i < nordenes - 1; i++)
    {
        if (pipe(fds) < 0)
        {
            perror("ERROR AL CREAR EL PIPE");
            return ERROR;
        }
        red_ordenes[i + 1].entrada = fds[0];
        red_ordenes[i].salida = fds[1];
    }

    return OK;
} // Fin de la funci�n "pipeline"

int redirigir_entrada(int i)
{
    if (dup2(red_ordenes[i].entrada, STDIN_FILENO) < 0)
    {
        perror("ERROR EN DUP2 DE LA ENTRADA");
        return ERROR;
    }
    return OK;
} // Fin de la funci�n "redirigir_entrada"

int redirigir_salida(int i)
{
    if (dup2(red_ordenes[i].salida, STDOUT_FILENO) < 0)
    {
        perror("ERROR EN DUP2 DE LA SALIDA");
        return ERROR;
    }
    return OK;
} // Fin de la funci�n "redirigir_salida"

int cerrar_fd(void)
{
    for (int i = 0; i < PIPELINE; i++)
    {
        if (red_ordenes[i].entrada > STDERR_FILENO) // Si la entrada es mayor a stderr significa que hay que cerrar algun descriptor
        {
            close(red_ordenes[i].entrada); // Lo cerramos
        }
        red_ordenes[i].entrada = -1; // Asignamos la entrada a -1 porque es un valor seguro

        if (red_ordenes[i].salida > STDERR_FILENO) // Si la salida es mayor a stderr significa que hay que cerrar algun descriptor
        {
            close(red_ordenes[i].salida); // Lo cerramos
        }
        red_ordenes[i].salida = -1; // Asignamos la salida a -1 porque es un valor seguro
    }
    return OK;
} // Fin de la funci�n "cerrar_fd"
