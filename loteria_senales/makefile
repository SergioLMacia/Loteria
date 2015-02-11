
CC= gcc
OBJ= main.o func.o
INC= func.h
PROG= sorteo
FILES= S%C
CFLAGS= -g -I. -Wall -o

# llamada por defecto
all: $(PROG)

$(PROG): $(OBJ)
	@$(CC) $(CFLAGS) $@ $^

%.o: %.c $(INC)
	@$(CC) -c $(CFLAGS) $@ $<


# Para eliminar
uninstall: clean cleanall
clean:
	@rm -f $(PROG) $(OBJ)

# Estas dos lineas eliminan los posibles ficheros existentes de loterias
cleanall: clean
	@find -type f -name 'S[0-9]R' -exec rm "{}" +
	@find -type f -name 'S[0-9][0-9]R' -exec rm "{}" +