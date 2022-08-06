# Makefile de simula_car

CC = gcc
# -Wall produce todos los avisos de una mal programación y -g produce información de depuración que utiliza gdb
CFLAGS = -Wall -g
OBJS = simula_car.o simula_car_adicional.o semaforo_con_espera_activa.o semaforo_sin_espera_activa.o

.PHONY: all
all: simula_car simula_car_con_espera_activa simula_car_sin_espera_activa

#  -lpthread enlaza con libpthread para que utilice la biblioteca de hilos
simula_car: simula_car.o
	$(CC) $(CFLAGS) simula_car.o -lpthread -o simula_car

simula_car_con_espera_activa: simula_car_adicional.o semaforo_con_espera_activa.o
	$(CC) $(CFLAGS) simula_car_adicional.o semaforo_con_espera_activa.o -lpthread -o simula_car_con_espera_activa

simula_car_sin_espera_activa: simula_car_adicional.o semaforo_sin_espera_activa.o
	$(CC) $(CFLAGS) simula_car_adicional.o semaforo_sin_espera_activa.o -lpthread -o simula_car_sin_espera_activa

simula_car.o: simula_car.c simula_car.h
	$(CC) $(CFLAGS) -D_REENTRANT -c simula_car.c

simula_car_adicional.o: simula_car_adicional.c simula_car.h semaforo.h
	$(CC) $(CFLAGS) -D_REENTRANT -c simula_car_adicional.c

semaforo_con_espera_activa.o: semaforo_con_espera_activa.c semaforo.h
	$(CC) $(CFLAGS) -c semaforo_con_espera_activa.c

semaforo_sin_espera_activa.o: semaforo_sin_espera_activa.c semaforo.h
	$(CC) $(CFLAGS) -c semaforo_sin_espera_activa.c

# Evita conflictos con un fichero que pueda llamarse clean
.PHONY: clean
# Elimina los ejecutables y archivos objetos
clean:
	rm -f $(OBJS) simula_car simula_car_con_espera_activa simula_car_sin_espera_activa


