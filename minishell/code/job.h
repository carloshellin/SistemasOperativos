/*
===============================================================================

   Fichero: job.h
   Descripción: Control de jobs
   
===============================================================================
*/

#ifndef JOB_H
#define JOB_H

#include <stdio.h>

enum estado_t
{
   RUNNING, STOPPED, FOREGROUND
};

struct job
{
   pid_t pid;
   int id;
   char actual;
   char orden[BUFSIZ];
   enum estado_t estado;
   struct job *siguiente; // Es una lista enlazada, por lo que esta estructura contiene un puntero al siguiente job
};

// Encuentra espacios a la izquierda y los elimina
void ignorar_espacios(char **buf);

// Comprueba si la orden es parte del control de jobs
int es_ord_job(char * orden);
// Ejecuta cualquier encontrada en el control de jobs
void ejecutar_ord_job(char *orden);

// Añade un nuevo job a la lista enlazada
void agregar_job(pid_t pid, const char *orden, int backgr);

// Elimina el job de la lista enlazada buscando el pid dado
void eliminar_job(pid_t pid);

// Muestra en una tabla todos los jobs encontrados en la lista enlazada
void mostrar_jobs();

// Devuelve el estado del job buscando el pid dado
enum estado_t estado_job(pid_t pid);

// Termina todos los jobs que se hayan quedado en la lista enlazada
void terminar_jobs();

// Devuelve el job cuyo estado sea FOREGROUND
struct job * foreground_job();

#endif