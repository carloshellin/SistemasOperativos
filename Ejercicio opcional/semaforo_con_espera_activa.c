/*
===============================================================================

   Fichero: semaforo_con_espera_activa.c
   Descripción: Implementación de los semáforos con espera activa
   
===============================================================================
*/

#include "semaforo.h"

void P(semaforo_t *s)
{
   // Operación atómica que decrementa en 1 al contador y devuelve el valor
   int contador = __atomic_sub_fetch(&s->contador, 1, __ATOMIC_RELAXED);
   if (contador < 0) // Si no es positiva
   {
      // Instrucción test_and_set que se ejecuta en modo atómico con espera activa.
      // Si valor es 0 le asigna 1 y devuelve 0, caso contrario devuelve 1
      while (__atomic_test_and_set(&s->valor, __ATOMIC_RELAXED));
   }
}

void V(semaforo_t *s)
{
   // Operación atómica que incrementa en 1 al contador y devuelve el valor
   int contador = __atomic_add_fetch(&s->contador, 1, __ATOMIC_RELAXED);
   if (contador <= 0) // Si no es positiva o es 0
   {
      // Instrucción clear que se ejecuta en modo atómico
      // y pone valor a 0 para que pueda continuar P()
      __atomic_clear(&s->valor, __ATOMIC_RELAXED);
   }
}