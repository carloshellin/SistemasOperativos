/*
===============================================================================

   Fichero: job.c
   Descripción: Control de jobs
   
===============================================================================
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "job.h"

struct job *jobs; // Cabecera de la lista enlazada

void background(int id)
{
   struct job *j;
   size_t orden_longitud;
   
   if (id)
   {
      for (j = jobs; j; j = j->siguiente)
      {
         if (j->id == id)
         {
            break; // Ha encontrado el job con el id dado y ya no hace falta seguir buscando
         }
      }
   }
   else
   {
      j = jobs; // Si no hay id, se obtiene como por defecto el job de la cabecera en la lista enlazada
   }
   
   if (j && j->estado == STOPPED)
   {
      j->estado = RUNNING;
      kill(j->pid, SIGCONT); // Envía la señal SIGGCONT al pid del job
      
      // Modifica la orden para añadir ' &' al final si hay espacio de memoria suficiente y así se muestre como "orden &"
      orden_longitud = strlen(j->orden);
      if (orden_longitud + 3 < BUFSIZ)
      {
         j->orden[orden_longitud] = ' ';
         j->orden[orden_longitud + 1] = '&';
      }
      
      printf("[%d]%c %s\n", j->id, j->actual, j->orden);
   }
}

void foreground(int id)
{
   struct job *j;
   size_t orden_longitud;
   
   if (id)
   {
      for (j = jobs; j; j = j->siguiente)
      {
         if (j->id == id)
         {
            break; // Ha encontrado el job con el id pedido y ya no hace falta seguir buscando
         }
      }
   }
   else
   {
      j = jobs; // Si no hay id, se obtiene como por defecto el job de la cabecera en la lista enlazada
   }
   
   if (j)
   {
      j->estado = FOREGROUND;
      kill(j->pid, SIGCONT); // Envía la señal SIGGCONT al pid del job
      
      // Elimina de la orden el '&' si es que existe
      orden_longitud = strlen(j->orden);
      if (j->orden[orden_longitud - 1] == '&')
      {
         j->orden[orden_longitud - 2] = '\0';
      }
      
      printf("%s\n", j->orden);
      
      pause(); // Como es una orden de primer plano esperamos por una señal como SIGTSTP y SIGINT o a que finalice la orden
   }
}

void ignorar_espacios(char **buf)
{
   char *pbuf;
   
   pbuf = *buf;
   while (*pbuf == ' ')
   {
      pbuf++; // Mueve el puntero cada vez que encuentra espacios ' '
   }
   
   *buf = pbuf;
}

int es_ord_job(char *orden)
{
   char buf[BUFSIZ];
   char *pbuf;
   
   strcpy(buf, orden);
   pbuf = buf;
   
   // Comprueba primero la orden sin tener en cuenta los posibles argumentos de fg o bg
   while (*pbuf >= 'a' && *pbuf <= 'z')
   {
      pbuf++;
   }
   *pbuf = '\0';
   
   if (!strcmp(buf, "jobs"))
   {
      return 1;
   }
   else if (!strcmp(buf, "fg"))
   {
      return 1;
   }
   else if (!strcmp(buf, "bg"))
   {
      return 1;
   }
   
   return 0;
}

void ejecutar_ord_job(char *orden)
{
   char buf[BUFSIZ];
   char *pbuf;
   char *delimitador;
   char *argv[BUFSIZ];
   int argc;
   int id;
   
   strcpy(buf, orden);
   pbuf = buf;
   
   argc = 0;
   argv[argc++] = pbuf;
   
   // Por cada delimitador de espacio ' ' después de la orden es un argumento más
   delimitador = strchr(pbuf, ' ');
   while (delimitador)
   {
      pbuf = delimitador + 1;
      argv[argc++] = pbuf;
      *delimitador = '\0';
      ignorar_espacios(&pbuf);
      delimitador = strchr(pbuf, ' ');
   }
   
   argv[argc] = '\0';
   
   if (argc >= 2) // Si hay dos argumentos (incluyendo la orden)
   {
      id = atoi(argv[1]); // Convierte el argumento 1 de string a int
   }
   
   if (!strcmp(argv[0], "jobs"))
   {
      mostrar_jobs();
   }
   else if (!strcmp(argv[0], "fg"))
   {
      foreground(id);
   }
   else if (!strcmp(argv[0], "bg"))
   {
      background(id);
   }
}

void agregar_job(pid_t pid, const char *orden, int backgr)
{
   struct job *j;
   struct job *ultimo;
   
   j = (struct job *) malloc(sizeof(struct job));
   j->pid = pid;
   
   if (!jobs) // Si no hay cabecera, este es el primero
   {
      j->id = 1;
      j->actual = '+'; // Signo '+' que indica que al usar bg o fg sin id el job por defecto es este
      jobs = j; // La cabecera es el primero
   }
   else
   {
      // Busca en la lista enlazada cuál es el último
      ultimo = jobs;
      while (ultimo->siguiente)
      {
         ultimo = ultimo->siguiente;
      }
      
      j->id = ultimo->id + 1; // Sabiendo el último sumamos su id + 1 para el nuevo job
      ultimo->siguiente = j; // El último tiene como siguiente el nuevo job
   }
   
   strcpy(j->orden, orden);
   j->estado = backgr ? RUNNING : FOREGROUND; // Dependiendo de si backgr es 1 el estado es RUNNING o 0 es FOREGROUND
   j->siguiente = NULL; // No hay siguiente para el nuevo job
}

void eliminar_job(pid_t pid)
{
   struct job *j;
   struct job *anterior;
   
   for (j = jobs; j; anterior = j, j = j->siguiente)
   {
      if (j->pid == pid)
      {
         if (j == jobs) // En caso de que el job encontrado sea la cabecera
         {
            jobs = j->siguiente; // La cabecera pasa ser el siguiente del job encontrado
            if (jobs && j->actual)
            {
               jobs->actual = '+'; // En caso de que fuese el actual, ahora la cabecera es el nuevo actual
            }
            free(j);
         }
         else // Sino es la cabecera
         {
            anterior->siguiente = j->siguiente; // El siguiente del anterior job pasa a ser el siguiente del job encontrado
            free(j);
         }
         
         return;
      }
   }
}

enum estado_t estado_job(pid_t pid)
{
   struct job *j;
   
   for (j = jobs; j; j = j->siguiente)
   {
      if (j->pid == pid)
      {
         return j->estado;
      }
   }
   
   return 0;
}

void mostrar_jobs()
{
   struct job *j;
   
   for (j = jobs; j; j = j->siguiente)
   {
      printf("[%d]%c ", j->id, j->actual);
      switch (j->estado)
      {
         case RUNNING:
         {
            printf("Running");
         } break;
         
         case STOPPED:
         {
            printf("Stopped");
         } break;
         
         default:
         {
            printf(" ");
         }
      }
      printf("\t\t%s\n", j->orden);
   }
}

void terminar_jobs()
{
   struct job *j;
   struct job *siguiente;
   
   for (j = jobs; j; j = siguiente)
   {
      siguiente = j->siguiente; // El siguiente de j para la próxima iteración del bucle
      kill(j->pid, SIGHUP);  // Envía la señal SIGUP al pid del job
   }
}

struct job * foreground_job()
{
   struct job *j;
   
   for (j = jobs; j; j = j->siguiente)
   {
      if (j->estado == FOREGROUND)
      {
         return j;
      }
   }
   
   return NULL;
}