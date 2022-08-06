/*
===============================================================================

   Fichero: minishell.c
   Descripción: Fichero principal con su punto de entrada (main) y el bucle de la minishell
   
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "internas.h"
#include "entrada_minishell.h"
#include "ejecutar.h"
#include "sig.h"
#include "job.h"

int main(int argc, char *argv[])
{
   char buf[BUFSIZ];
   char *orden; 
   char *saveptr; // Puntero que utiliza internamente strtok_r para mantener el contexto entre diferentes llamadas
   int bucle;
   
   iniciar_signals();
   
   bucle = 1;
   while (bucle)
   {
      imprimir_prompt();
      if (!leer_linea_ordenes(buf)) // Espera a que el usuario introduzca una orden. La orden se almacena en buf.      	      
      {
         printf("exit\n");
         break;
      }
      
      orden = strtok_r(buf, ";", &saveptr); // "strtok_r", función reentrante que coge la variable "buf"; cada vez que encuentre un ";" separará esta variable en trozos llamados tokens y devuelve el puntero al siguiente token en "orden".
      
      while (orden)
      {     
         ignorar_espacios(&orden);
         
         if (!strcmp(orden, "exit")) // "strcmp" compara la orden buf con el string "exit" caracter por caracter. Si son iguales, devuelve valor 0. Si no, 1.
         {   
            bucle = 0;
            break;// Si la orden introducida es igual a "exit", sale del bucle.
         }
         else // Si la orden introducida es distinta a "exit"...
         {
            if (es_ord_interna(orden))
            {
               ejecutar_ord_interna(orden);
            }  
            else if (es_ord_job(orden))
            {
               ejecutar_ord_job(orden);
            }
            else 
            {
               ejecutar_linea_ordenes(orden);
            }
         }
         
	      orden = strtok_r(NULL, ";", &saveptr);
      }
   }
   
   terminar_jobs();
   
   return 0;
}
