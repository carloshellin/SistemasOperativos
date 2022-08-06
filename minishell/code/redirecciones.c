/*
===============================================================================

   Fichero: redirecciones.c
   Descripción: Redireccionar la entrada y/o salida
   
===============================================================================
*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>


// función que abre el archivo situado en la posición indice_entrada+1
// de la orden args y elimina de ella la redirección completa 

void redirec_entrada(char **args, int indice_entrada, int *entrada)
{
   *entrada = open(args[indice_entrada + 1], O_RDONLY); // Asigna a entrada el valor del descriptor de archivo
   // El flag es O_RDONLY porque al ser archivo de entrada solo se usará para leerlo.
   if (*entrada == -1)
   {
      perror(args[indice_entrada + 1]); // Se enviará un error en caso de que el valor del descriptor sea -1
   }
   
   // Libera de memoria las partes que free_argumentos no podría hacer
   free(args[indice_entrada]);
   free(args[indice_entrada + 1]);
   
   args[indice_entrada] = NULL;
}

// función que abre el archivo situado en la posición indice_salida+1
// de la orden args y elimina de ella la redirección completa          

void redirec_salida(char **args, int indice_salida, int *salida)
{
   *salida = open(args[indice_salida + 1], O_RDWR | O_CREAT | O_TRUNC, 0666); // Asigna a salida el descriptor de archivo
   // Los flags son para poder leer y escribir, crear el archivo si no existe y truncarlo a longitud 0
   // Los permisos son teniendo en cuenta la máscara del usuario para ficheros, ya que open hace (0666 & ~umask)
   if (*salida == -1)
   {
      perror(args[indice_salida + 1]); // Error en caso de que salida sea -1
   }
   
   // Libera de memoria las partes que free_argumentos no podría hacer
   free(args[indice_salida]);
   free(args[indice_salida + 1]);
   
   args[indice_salida] = NULL;
}
