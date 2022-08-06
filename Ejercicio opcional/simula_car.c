/*
===============================================================================

   Fichero: simula_car.c
   Descripción: Simula una carrera de coches con el uso de hilos y semáforos
   
===============================================================================
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "simula_car.h"

/* Variables globales */

// Array de datos de tipo coche_t
coche_t Coches[N_COCHES];

// Array con el id de cada coche que ha ido llegando
volatile int clasificacionFinal[N_COCHES];
// Bool para saber si la carrera ha finalizado o no
volatile int finalCarrera = 0;

// Variable global inicializada con unos atributos por defecto
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
   
   // Si mutex está desbloqueado, lo bloquea y continúa con la ejecución.
   // En caso contrario, se suspende el hilo a la espera de que otro hilo desbloquee el mutex.
   if (pthread_mutex_lock(&mutex))
   {
      // Si hay un error, se muestra y finaliza el hilo en estado -1
      perror("pthread_mutex_lock");
      pthread_exit((void *) -1);
   }
   
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
   // Desbloquea el mutex y si hay hilos en espera permite que puedan bloquearlo de nuevo
   if (pthread_mutex_unlock(&mutex))
   {
      // Si hay un error, se muestra y finaliza el hilo en estado -1
      perror("pthread_mutex_unlock");
      pthread_exit((void *) -1);
   }
   
   // Finaliza la ejecución de hilo, devuelvo 0 al estado
   pthread_exit(0);
}


int main(void)
{
   pthread_t hilosCoches[N_COCHES]; // tabla con los identificadores de los hilos
   int i;
   
   // Se inicializa el semáforo 
   if (pthread_mutex_init(&mutex, NULL))
   {
      perror("pthread_mutex_init");
      exit(EXIT_FAILURE);
   }
   
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
   
   // Destruye el semáforo y libera la memoria de mutex
   if (pthread_mutex_destroy(&mutex))
   {
      perror("pthread_mutex_destroy");
      exit(EXIT_FAILURE);
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