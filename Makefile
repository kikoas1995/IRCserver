
CC	= gcc

CFLAGS	= -Wall -g


DIR	= ~/G-2311-02-P1
TAR = G-2311-02-P1


EJECUTABLES	= server
MAIN		= main.o
CONNECTSERVER   = connectServer.o
LIBS      	= -lircredes -lirctad -lpthread
EXEC_NAME 	= servidor
SRC			= src/
SRCLIB			= srclib/
INCL		= includes/
OBJ			= obj/
LIB		= lib/


all: dist $(EJECUTABLES) autores

server:    $(OBJ)$(MAIN) $(LIB)libConnectServer.a
	$(CC) -o $(EXEC_NAME) $(OBJ)$(MAIN) $(LIB)libConnectServer.a $(LIBS)



$(OBJ)IRCcommands.o: $(SRCLIB)IRCcommands.c
	$(CC) -c $< -o $@

$(OBJ)connectServer.o: $(SRCLIB)connectServer.c
	$(CC) -c $< -o $@


$(OBJ)main.o: $(SRC)main.c
	$(CC) -c $< -o $@ 

$(LIB)libConnectServer.a: $(OBJ)connectServer.o $(OBJ)IRCcommands.o
	ar r $(LIB)libConnectServer.a $(OBJ)connectServer.o $(OBJ)IRCcommands.o
#============================================================
# Crea un archivo .tgz para distribuir el codigo
#============================================================
##Crea un archivo .tgz elmininando previamente el codigo objeto

dist: clean
	#@ cp $(DIR)/Makefile $(DIR)/G-2311-02-P1-makefile
	tar -cvf $(TAR).tar.gz ../$(TAR)


#============================================================
# Limpia los ficheros temporales
#============================================================

clean:
	#rm -f $(EXEC_NAME) $(MAIN) $(CONNECTSERVER)
	rm -f $(EXEC_NAME) all $(OBJ)*.o $(LIB)*.a $(DIR)/*~ core $(DIR)/Makefile

#============================================================
# Mensaje que aparece al ejecutar el MAKE, y que ayuda a identificar
# a los autores del trabajo
#============================================================

autores:
	@echo "-----------------------------------------------------------------------"
	@echo "Practica Redes2 grupo 2311 desarrollado por el equipo 02 compuesto por:"
	@echo "Andreu Sanz, Francisco y Martínez Hernández, Javier"
	@echo "-----------------------------------------------------------------------"

#============================================================
# Ayuda
#============================================================

ayuda:
	@echo Uso: make [objetivo]
	@echo "make \t\t Make all"
	@echo "make dist \t Genera el archivo de distribucion .tgz"
	@echo "make limpia \t Limpia los ficheros temporales"
