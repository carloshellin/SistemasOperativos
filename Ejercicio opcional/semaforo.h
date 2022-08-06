/*
===============================================================================

   Fichero: semaforo.h
   Descripción: Datos y funciones prototipo de los semáforos
   
===============================================================================
*/

#ifndef SEMAFORO_H
#define SEMAFORO_H

// Tipo de datos que representa un semáforo
typedef struct {
   int contador;
   int valor;
   int futex;
} semaforo_t;

// Operación P del semáforo
void P(semaforo_t *s);

// Operación V del semáforo
void V(semaforo_t *s);


#endif