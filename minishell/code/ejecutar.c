/*
===============================================================================

   Fichero: ejecutar.c
   Descripción: Ejecutar órdenes externas en la minishell
   
===============================================================================
*/

#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#include "parser.h"
#include "ejecutar.h"
#include "libmemoria.h"

#include "redirecciones.h"
#include "job.h"

int **crear_pipes(int nordenes)
{
   int **pipes = NULL;
   int i;
   
   pipes = (int **) malloc(sizeof(int *) * (nordenes - 1)); // Se reserva memoria dinámica de tipo int** del tamaño de un puntero de tipo int por el número de órdenes - 1 
   for (i = 0; i < (nordenes - 1); i++)
   {
      pipes[i] = (int *) malloc(sizeof(int) * 2); // Se reserva memoria dinámica que almacena los descriptores para leer y escribir en una tubería
      pipe(pipes[i]); // Crea la tubería y la almacena en pipes[i]
   }
   return(pipes);
}

pid_t ejecutar_orden(const char *orden, int entrada, int salida, int *pbackgr)
{
   char **args; // Puntero a punteros de tipo de char para almacenar los argumentos
   pid_t pid; // El identificador del proceso
   
   int indice_ent = -1, indice_sal = -1; // Por defecto, no hay ni "<" ni ">"
   
   if ((args = parser_orden(orden, &indice_ent, &indice_sal, pbackgr)) == NULL) // Convierte la cadena orden a una más adecuada para exec
   {
      if (entrada != STDIN_FILENO)
      {
         close(entrada); // Desecha el descriptor de archivo de entrada si es diferente a STDIN_FILENO
      }
      
      if (salida != STDOUT_FILENO)
      {
         close(salida); // Desecha el descriptor de archivo de salida si es diferente a STDOUT_FILENO
      }
      
      
      return(-1); // Devuelve -1 si parser_orden es NULL
   }
   
   if (indice_ent != -1)
   {
      redirec_entrada(args, indice_ent, &entrada); // Abre el archivo entrada y elimina las redirecciones de la orden
      if (entrada == -1)
      {
         free_argumentos(args);
         return -1;
      }
   }
   if (indice_sal != -1)
   {
      redirec_salida(args, indice_sal, &salida); // Abre el archivo salida y elimina las redirecciones de la orden
      if (salida == -1)
      {
         free_argumentos(args);
         return -1;
      }
   }
   
   pid = fork(); // Se crea un nuevo proceso duplicando la minishell
   if (pid == 0) // Se comprueba si es la minishell hija
   {
      if (entrada != STDIN_FILENO) // Si el descriptor de archivo de entrada es distinto al predeterminado (por la existencia de una redirección)
      {
         dup2(entrada, STDIN_FILENO); // Realiza una copia del descriptor de archivo reemplazando el descriptor de entrada predeterminado por este
         close(entrada); // Desecha el descriptor de archivo inicial
      }
      if (salida != STDOUT_FILENO) // Si el descriptor de archivo de salida es distinto al predeterminado (por la existencia de una redirección)
      {
         dup2(salida, STDOUT_FILENO); // Realiza una copia del descriptor de archivo reemplazando el descriptor de entrada predeterminado por este
         close(salida); // Desecha el descriptor de archivo inicial
      }
      
      if (execvp(args[0], args) == -1) // Ejecuta la orden almacenada en args[0] junto a sus argumentos
      {
		   perror(args[0]); // Muestra el mensaje de error correspondiente al valor errno
		   exit(-1); // Termina el proceso minishell hija indicando de que ha fallado con un -1
      }
   }
   else if (pid == -1)
   {
      perror("fork()");
   }
   else // Si no es la minishell hija
   {
      if (entrada != STDIN_FILENO) 
      {
         close(entrada);
         // Desecha el descriptor de archivo de entrada si es diferente a STDIN_FILENO
      }
      
      if (salida != STDOUT_FILENO)
      {
         close(salida);
         // Desecha el descriptor de archivo de entrada si es diferente a STDIN_FILENO
      }
      
      free_argumentos(args); // Libera de memoria dinámica todos los argumentos
   }
   
   return pid; // Devuelve el pid de la minishell hija
}

void ejecutar_linea_ordenes(const char *orden)
{
   char **ordenes;
   int nordenes;
   pid_t *pids = NULL; // Los identificadores de los procesos
   int **pipes;
   int backgr; // Puede ser 1, entonces es un proceso "background" o 0 que no lo es
   int i;
   int entrada = STDIN_FILENO, salida = STDOUT_FILENO; // Varibles que indican los descriptores de archivos a los que deben ser redireccionadas la entrada y la salida
   
   ordenes = parser_pipes(orden, &nordenes); // Variable que almacena en un char** las distintas órdenes simples
   
   pipes = crear_pipes(nordenes); // Variable que almacena tantas tuberías como número de órdenes - 1
   
   pids = (pid_t *) malloc(sizeof(pid_t *) * nordenes); // Variable que almacena el puntero a memoria dinámica de tipo pid_t
   
   for (i = 0; i <= nordenes - 1; i++)
   {
      if (i == 0)
      {
         if (nordenes > 1)
         {
            salida = pipes[0][1]; // Si hay más de una orden, el valor de la salida se iguala a la escritura de la primera tubería
         }
         else
         {
            salida = STDOUT_FILENO; // Si no hay más de una orden, la salida se iguala a STDOUT_FILENO
         }
         
         pids[0] = ejecutar_orden(ordenes[i], STDIN_FILENO, salida, &backgr);
         
         if (pids[0] > 0 && nordenes == 1) // Solo para una orden
         {
            agregar_job(pids[0], orden, backgr);
         }
      }
      else if (i == nordenes - 1 && nordenes > 1) // Si hay más de una orden e i tiene el valor del número de órdenes -1
      {
         entrada = pipes[nordenes - 2][0]; // Cambia el valor de entrada por el de la lectura de la última tubería
         
         pids[i] = ejecutar_orden(ordenes[i], entrada, STDOUT_FILENO, &backgr);
      }
      else
      {
         entrada = pipes[i - 1][0]; // Cambia el valor de entrada por el de lectura de la tubería que indique i - 1
         salida = pipes[i][1]; // Cambia el valor de la salida por el de escritura de la siguiente tubería que indique i
         
         pids[i] = ejecutar_orden(ordenes[i], entrada, salida, &backgr);
      }
   }
   
   if (!backgr && pids[nordenes - 1] > 0) // Si no existe "background" y el pid de la última orden (nordenes - 1) es la minishell hija
   {
      if (nordenes > 1)
      {
         waitpid(pids[nordenes - 1], 0, 0); // Espera a que termine la minishell hija
      } else {
         pause(); // Como es una orden de primer plano esperamos por una señal como SIGTSTP y SIGINT o a que finalice la orden
      }    
   }
   
   // liberar estructuras de datos dinamicas utilizadas
   free(pids);
   free_ordenes_pipes(ordenes, pipes, nordenes);
}
