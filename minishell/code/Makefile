# Makefile de la minishell

CC = gcc
# -Wall produce todos los avisos de una mal programación y -g produce información de depuración que utiliza gdb
CFLAGS = -Wall -g 
OBJS = minishell.o ejecutar.o entrada_minishell.o libmemoria.o redirecciones.o sig.o job.o

.PHONY: all
all: minishell

# -L ./ busca primero las posibles bibliotecas en el directorio actual y -lshell enlaza con libshell.a
minishell: $(OBJS) libshell.a
	$(CC) $(CFLAGS) $(OBJS) -L ./ -lshell -o minishell

# Con -c se detiene gcc en la fase de generación del objeto para luego enlazar con todos los .o y la biblioteca estática
minishell.o: minishell.c entrada_minishell.h internas.h ejecutar.h sig.h job.h
	$(CC) $(CFLAGS) -c minishell.c

ejecutar.o: ejecutar.c ejecutar.h parser.h libmemoria.h redirecciones.h job.h
	$(CC) $(CFLAGS) -c ejecutar.c

entrada_minishell.o: entrada_minishell.c entrada_minishell.h
	$(CC) $(CFLAGS) -c entrada_minishell.c

libmemoria.o: libmemoria.c libmemoria.h
	$(CC) $(CFLAGS) -c libmemoria.c

redirecciones.o: redirecciones.c
	$(CC) $(CFLAGS) -c redirecciones.c

sig.o: sig.c job.h
	$(CC) $(CFLAGS) -c sig.c

job.o: job.c job.h
	$(CC) $(CFLAGS) -c job.c

# Crea la biblioteca estática .a con -r para insertar los .o y -s genera un índice con cada símbolo
libshell.a: parser.o internas.o
	ar -rs libshell.a parser.o internas.o

# Evita conflictos con un fichero que pueda llamarse clean
.PHONY: clean
# Elimina el ejecutable, la biblioteca y los archivos objetos excepto parser.o e internas.o para poder crear libshell.a
clean:
	rm -f $(OBJS) minishell libshell.a