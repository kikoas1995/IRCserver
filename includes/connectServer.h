/*************************************************************
 * Proyecto: REDES II
 * Fichero: connectServer.h
 * Fecha: 04/02/2016
 * Descripcion: Declaracion de las funciones que inician la conexion al servidor
 * Integrantes: Francisco Andreu Sanz, Javier Martínez Hernández
 *************************************************************/

#include "cabeceras.h"
#include "IRCcommands.h"

#ifndef CONNECTSERVER_H_
#define CONNECTSERVER_H_  

int do_daemon(char *nombre);
int initiate_server(char *nombre);
int abrirSocket();
int abrirBind(int sockval, int puerto);
int abrirListen(int sockval, int tam);
char* UnPipeline(char *string, char **command, char *str_r);
long ejecutarAccion(long option, char* command, char** nick, int sd);

#endif /* CONNECTSERVER_H_ */
