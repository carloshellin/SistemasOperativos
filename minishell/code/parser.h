/*
===============================================================================

   Fichero: parser.h
   Descripción: Funciones para hacer el parsing (análisis sintáctico) de las ordenes
   
===============================================================================
*/

#ifndef PARSER_H
#define PARSER_H
enum TEstado {
   INICIO_ARG, ARG
};

// Hace el "parsing" de orden con tuberías y devuelve el número de órdenes
char ** parser_pipes (const char *orden, int *numordenes);

// Convierte la cadena orden a una más adecuada para exec
char ** parser_orden (const char *orden, int *indentrada, int *indsalida, int *background);


#endif 