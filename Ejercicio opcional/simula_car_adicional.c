/*
===============================================================================

   Fichero: simula_car_adicional.c
   Descripción: Simula una carrera de coches con el uso de hilos y semáforos implementados
   
===============================================================================
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "simula_car.h"
#include "semaforo.h"

/* Variables globales */

// Array de datos de tipo coche_t
coche_t Coches[N_COCHES];

// Array con el id de cada coche que ha ido llegando
volatile int clasificacionFinal[N_COCHES];
// Bool para saber si la carrera ha finalizado o no
volatile int finalCarrera = 0;

// Variable global para el semáforo
semaforo_t s;

// Funcion ejecutada por los hilos
void *funcion_coche(void *arg)
{
   coche_t *pcoche = (coche_t *) arg; // Casting de void * a coche_t *
   int aleatorio;
   unsigned int semilla = (pcoche->id) + getpid(); // semilla generacion num. aleatorios
   int i;
   
   printf("Salida de %s %d\n", pcoche->cadena, pcoche->id);
   
   fflush(stdout);
   
   // generar numero aleatorios con funcion re-entrante rand_r()    
   aleatorio = rand_r(&semilla) % 10;
   
   sleep(aleatorio);
   
   printf("Llegada de %s %d\n", pcoche->cadena, pcoche->id);
   
   // Operación P del semáforo
   P(&s);
   
   // Sección critica
   for (i = 0; i < N_COCHES; i++)
   {
      if (i == N_COCHES - 1)
      {
         // En caso de ser último coche en llegar, se da por finalizada la carrera
         finalCarrera = 1;
      }
      
      if (!clasificacionFinal[i])
      {
         clasificacionFinal[i] = pcoche->id;
         break;
      }
   }
   
   // Operación V del semáforo
   V(&s);
   
   // Finaliza la ejecución de hilo, devuelvo 0 al estado
   pthread_exit(0);
}


int main(void)
{
   pthread_t hilosCoches[N_COCHES]; // tabla con los identificadores de los hilos
   int i;
   
   // Se inicializa el semáforo
   s.contador = 1;
   s.valor = 1;
   
   printf("Se inicia proceso de creacion de hilos...\n\n");
   printf("SALIDA DE COCHES\n");
   
   for (i = 0; i < N_COCHES; i++)
   {
      Coches[i].id = i + 1;
      Coches[i].cadena = "Coche";
      // Crea un hilo para cada coche y cada hilo ejecuta funcion_coche
      if (pthread_create(&hilosCoches[i], NULL, funcion_coche,  &Coches[i]))
      {
         perror("pthread_create");
         exit(EXIT_FAILURE);
      }
   }
   
   printf("Proceso de creacion de hilos terminado\n\n");
   
   
   for (i = 0; i < N_COCHES; i++)
   {
      // Espera a la terminación de cada hilo
      if (pthread_join(hilosCoches[i], NULL))
      {
         perror("pthread_join");
         exit(EXIT_FAILURE);
      }
   }
   
   printf("Todos los coches han LLEGADO A LA META \n");
   
   // Al finalizar la carrera, se muestra la clasificación
   if (finalCarrera)
   {
      printf("Clasificación:\n");
      printf("Posición\tCoche\n");
      for (i = 0; i < N_COCHES; i++)
      {
         printf("%i\t\t %i\n", i + 1, clasificacionFinal[i]);
      }
   }
   
   return 0;
}


