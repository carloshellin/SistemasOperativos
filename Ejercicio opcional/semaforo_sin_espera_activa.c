/*
===============================================================================

   Fichero: semaforo_sin_espera_activa.c
   Descripción: Implementación de los semáforos sin espera activa
   
===============================================================================
*/

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <sched.h>

#include "semaforo.h"

void P(semaforo_t *s)
{
   // Operación atómica que decrementa en 1 al contador y devuelve el valor
   int contador = __atomic_sub_fetch(&s->contador, 1, __ATOMIC_RELAXED);
   if (contador < 0)  // Si no es positiva
   {
      // Llamada del sistema a futex con el argumento FUTEX_WAIT si el 
      // valor del futex es igual al valor del cuarto argumento
      // suspende el proceso y lo agrega a la cola de bloqueados del 
      // futex. Devuelve 0 si fue desbloqueado con FUTEX_WAKE
      syscall(SYS_futex, &s->futex, FUTEX_WAIT, s->futex, NULL, 0, 0);
   }
}

void V(semaforo_t *s)
{
   // Operación atómica que incrementa en 1 al contador y devuelve el valor
   int contador = __atomic_add_fetch(&s->contador, 1, __ATOMIC_RELAXED);
   if (contador <= 0)  // Si no es positiva o es 0
   {
      // Llamada del sistema a futex con el argumento FUTEX_WAKE que 
      // desbloquea a un proceso según lo indicado por el cuarto 
      // argumento. Devuelve el número de procesos desbloqueados
      while (syscall(SYS_futex, &s->futex, FUTEX_WAKE, 1, NULL, 0, 0) < 1) {
         // Llamada al sistema para que el núcleo quite de ejecución al 
         // proceso y lo mueve a la cola de listos (sin espera activa)
         sched_yield();
      }
   }
}

