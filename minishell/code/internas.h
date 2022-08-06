/*
===============================================================================

   Fichero: internas.h
   Descripción: Funciones para las ordenes internas de la minishell
   
===============================================================================
*/

#ifndef INTERNAS_H
#define INTERNAS_H

struct tipo_interna
{
   char *nombre;
   void (*func)(const char *);
};

// Comprueba si la orden es interna
int es_ord_interna(const char *);

// Ejecuta la orden interna
void ejecutar_ord_interna(const char *);

#endif