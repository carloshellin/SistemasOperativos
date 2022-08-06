/*
===============================================================================

   Fichero: sig.c
   Descripción: Manejadores de señales
   
===============================================================================
*/

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#include "job.h"

static void manejar_sigchild(int signo)
{
   int estado; // Variable que contiene el estado del proceso.
   pid_t pid;
   
   pid = waitpid(-1, &estado, WNOHANG); // Espera la finalización de cualquier proceso hijo, ya que especifica que el pid es -1; a la vez, almacena el código del estado de terminación del proceso en la variable "estado". Además waitpid no bloqueará el proceso padre, debido a la opción WNOHANG, que volverá inmediatamente (espera no bloqueante)
   
   if (pid > 0)
   {
      eliminar_job(pid);
   }
}

static void manejar_sigtstp(int signo)
{
   struct job *j = foreground_job();
   
   printf("\n");
   
   if (j && j->pid > 0)
   {
      j->estado = STOPPED;
      printf("[%d]%c %s\t\t%s\n", j->id, j->actual, "Stopped", j->orden);
      kill(j->pid, SIGTSTP); // Envía la señal SIGTSTP al pid del job
   }
}

static void manejar_sigint(int signo)
{
   struct job *j = foreground_job();
   
   printf("\n");
   
   if (j && j->pid > 0)
   {
      
      kill(j->pid, SIGINT); // Envía la señal SIGINT al pid del job
   }
}

void iniciar_signals()
{
   struct sigaction sa;
   struct sigaction satstp;
   struct sigaction saint;
   
   memset(&sa, 0, sizeof(sa)); // Inicializa los elementos de la estructura "sa" en "0".
   sa.sa_handler = manejar_sigchild; // Iguala la variable "sa_handler", encargada de proporcionar la acción que se va a asociar con la señal, a la función manejar_sigchild.
   sa.sa_flags = SA_NOCLDSTOP | SA_RESTART; // "sa_flags" especifica una serie de opciones, las cuales son "SA_NOCLDSTOP" y "SA_RESTART". SA_NOCLDSTOP hará que si la señal es "SIGCHLD" no se reciba ninguna notificación cuando el proceso hijo se pare. SA_RESTART hará re-ejecutables algunas llamadas al sistema entre señales.
   
   memset(&satstp, 0, sizeof(satstp));
   satstp.sa_handler = manejar_sigtstp;
   satstp.sa_flags = SA_RESTART;
   
   memset(&saint, 0, sizeof(saint));
   saint.sa_handler = manejar_sigint;
   saint.sa_flags = SA_RESTART;
   
   // Configura las señales
   sigaction(SIGCHLD, &sa, NULL); 
   sigaction(SIGTSTP, &satstp, NULL);
   sigaction(SIGINT, &saint, NULL);
}