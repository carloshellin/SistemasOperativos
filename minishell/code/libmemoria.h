/*
===============================================================================

   Fichero: libmemoria.h
   Descripción: Liberar de la memoria dinámica
   
===============================================================================
*/

#ifndef LIBMEMORIA_H
#define LIBMEMORIA_H

// Libera los argumentos de la memoria dinámica
void free_argumentos(char **args);

// Libera las órdenes y las tuberías de la memoria dinámica
void free_ordenes_pipes(char **ordenes, int **fds, int nordenes);

#endif 