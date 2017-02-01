/*************************************************************
* Proyecto: REDES II
* Fichero: cabeceras.h
* Fecha: 04/02/2016
* Descripcion: Declaracion de las cabeceras y definiciones del proyecto
* Integrantes: Francisco Andreu Sanz, Javier Martínez Hernández
*************************************************************/

#ifndef CABECERAS_H_
#define CABECERAS_H_ 

#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <syslog.h> 
#include <redes2/irc.h>
#include <pthread.h>
#include <sys/socket.h>
#include <time.h>

typedef struct thread_info thread_info;
typedef struct sock_lst sock_lst;

struct thread_info {
	int sd;
    int client_id;     
	struct sockaddr_in address;
    int addrlen;
};

struct sock_lst{
    int id;
    int closed;
};

#define TRUE   1
#define FALSE  0
#define PORT 6667
#define MAX_CONNECTIONS 20
#define MAX_CLIENTS 20

int client_socket[MAX_CLIENTS];
int closed[MAX_CLIENTS];
char* nickList[MAX_CLIENTS];
char *SERVER_NOMBRE;

#endif /* CABECERAS_H_ */
