/*
===============================================================================

   Fichero: simula_car.h
   Descripción: Simula una carrera de coches con el uso de hilos y semáforos
   
===============================================================================
*/

#ifndef SIMULA_CAR_H
#define SIMULA_CAR_H

#define N_COCHES 8

// Tipo de datos que representa un coche
typedef struct {
   int id;
   char *cadena;
} coche_t;

#endif