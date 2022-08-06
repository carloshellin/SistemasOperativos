/*
===============================================================================

   Fichero: redirecciones.h
   Descripción: Redireccionar la entrada y/o salida
   
===============================================================================
*/

#ifndef REDIRECCIONES_H
#define REDIRECCIONES_H

// Abre el archivo para entrada y elimina las redirecciones de la orden
void redirec_entrada(char **args, int indice_entrada, int *entrada);

// Abre el archivo para salida y elimina las redirecciones de la orden
void redirec_salida(char **args, int indice_salida, int *salida);

#endif
