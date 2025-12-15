/*-----------------------------------------------------+
 |     U S H. C
 +-----------------------------------------------------+
 |     Versión :                                       |
 |     Autor      :  SERGIO REAL GONZALVO              |
 |     Asignatura :  SOP-GIIROB                        |
 |     Descripción :                                   |
 +-----------------------------------------------------*/
#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include "ejecucion.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "profe.h"

#include <stdio.h>
#include <stdbool.h>

//
// Declaraciones de funciones locales
//
void visualizar(void);
int leerLinea(char *linea, int maxLinea);

//
// Prog. ppal.
//
int main(int argc, char *argv[])
{
  char buffer1[255] = "";
  char buffer2[255] = "";
  char *ant[2] = {buffer1, buffer2};
  char line[255];
  int res;
  char **m_ordenes;
  char ***m_argumentos;
  int *m_num_arg;
  int m_n;

  while (1)
  {
    do
    {
      res = leerLinea(line, MAXLINE);
      if (res == -2)
      {
        fprintf(stdout, "logout\n");
        exit(0);
      }

      if (res == -1)
      {
        fprintf(stdout, "linea muy larga\n");
      }
    } while (res <= 0);

    if (analizar(line) == OK)
    {
      m_n = num_ordenes();
      m_num_arg = num_argumentos();
      m_ordenes = get_ordenes();
      m_argumentos = get_argumentos();
      if (m_n > 0)
      {
        if (pipeline(m_n, fich_entrada(), fich_salida(), es_append(), es_background()) == OK)
          ejecutar(m_n, m_num_arg, m_ordenes, m_argumentos, es_background(), ant);
      }

      visualizar();
    }
  }
  return 0;
}

/****************************************************************/
/*                       leerLinea
  --------------------------------------------------------------

   DESCRIPCIÓN:
   Obtiene la línea de órdenes para el mShell.
   Util para depuracion.

   ENTRADA:
    linea - puntero a un vector de carácteres donde se almancenan los caracteres
   leídos del teclado
    tamanyLinea - tamaño máximo del vector anterior

   SALIDA:
    -- linea - si termina bien, contiene como último carácter el retorno de carro.
    -- leerLinea -  Entero que representa el motivo de finalización de la función:
     > 0 - terminación correcta, número de caracteres leídos, incluído '\n'
     -1 - termina por haber alcanzado el número máximo de caracteres que se
    espera leer de teclado, sin encontrar '\n'.
     -2 - termina por haber leído fin de fichero (EOF).
*/
/****************************************************************/

int leerLinea(char *linea, int maxLinea)
{
  char *ans;
  int c;
  ans = getcwd(NULL, 0);
  int count = 0;

  printf("Directorio actual: %s\n", ans);
  printf("Prompt %s ", PROMPT);
  fflush(stdout); // mostrar el prompt
  free(ans);      // liberar memoria

  c = getchar();

  while (c != '\n' && c != EOF)
  {
    if (count >= maxLinea - 1)
    {
      linea[count] = '\0';
      while (getchar() != '\n')
        ;
      return -1;
    }
    linea[count] = c;
    count++;
    c = getchar();
  }

  if (c == EOF && count == 0)
  {
    return -2;
  }

  if (c == '\n')
  {
    linea[count] = '\n';
    count++;
  }

  linea[count] = '\0';
  return count;
}

/****************************************************************/
/*                       visualizar                             */
/*--------------------------------------------------------------*/
/*                                                              */
/* DESCRIPCIÓN:                                                 */
/* Visualiza los distintos argumentos de la orden analizada.    */
/* Util para depuracion.                                        */
/*                                                              */
/* ENTRADA: void                                                */
/*                                                              */
/* SALIDA: void                                                 */
/*                                                              */
/****************************************************************/
void visualizar(void)
{
  printf(
      "\n"
      "==========================================================================\n"
      "                  VISUALIZADOR DE SERGIO REAL GONZALVO                    \n"
      "==========================================================================\n"
      "\n");
  printf("Nº ordenes: %d\n", num_ordenes());
  printf("+-----------+-------+-----------------+----------+----------+------------+------------+\n");
  printf("| %-9s | %5s | %-15s | %-8s | %-8s | %-10s | %-10s |\n",
         "Ordenes", "Args", "Argumentos", "Entrada", "Salida", "Modo", "Ejecución");
  printf("+-----------+-------+-----------------+----------+----------+------------+------------+\n");

  for (int i = 0; i < num_ordenes(); i++)
  {
    // Buffer para los argumentos
    char argumentos_str[100] = "";
    // Concatenar todos los argumentos en una cadena
    for (int j = 0; j < num_argumentos()[i]; j++)
    {
      if (j > 0)
      {
        strcat(argumentos_str, " ");
      }
      strcat(argumentos_str, get_argumentos()[i][j]);
    }
    // Si la cadena es muy larga, truncarla
    if (strlen(argumentos_str) > 15)
    {
      argumentos_str[12] = '.';
      argumentos_str[13] = '.';
      argumentos_str[14] = '.';
      argumentos_str[15] = '\0';
    }
    // Inicializo modo_salida para comprobar el append y trunk
    char *modo_salida;
    // Compruebo que no hay archivo de salida
    if (fich_salida() == NULL || fich_salida()[0] == '\0')
    {
      modo_salida = "";
    }
    // Es append
    else if (es_append())
    {
      modo_salida = "\033[31mAPPEND    \033[0m";
    }
    // No es append entonces es trunk
    else
    {
      modo_salida = "\033[32mTRUNK     \033[0m";
    }
    printf("| %-9s | %5d | %-15s | %-8s | %-8s | %-10s | %-10s |\n",
           get_ordenes()[i],
           num_argumentos()[i],
           argumentos_str,
           fich_entrada() ? fich_entrada() : "stdin",
           fich_salida() ? fich_salida() : "stdout",
           modo_salida,
           es_background() ? "background" : "foreground");
  }

  printf("+-----------+-------+-----------------+----------+----------+------------+------------+\n");

} // Fin de "visualizar"