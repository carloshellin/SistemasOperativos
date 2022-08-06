/*
===============================================================================

   Fichero: entrada_minishell.h
   Descripción: Manipulación de la entrada en la minishell
   
===============================================================================
*/

#ifndef ENTRADA_MINISHELL_H
#define ENTRADA_MINISHELL_H

// Lee las ordenes que se introducen en stdin
int leer_linea_ordenes(char *cad);

// Muestra el prompt en pantalla
void imprimir_prompt();

#endif
